#include "core/hotkey/globalshortcut.h"

#include <QGuiApplication>
#include <QSocketNotifier>

#ifdef NOX_HAS_X11
#include <X11/Xlib.h>
#include <X11/keysym.h>
#endif

GlobalShortcut::GlobalShortcut(QObject *parent)
    : QObject(parent)
{
    setup();
}

GlobalShortcut::~GlobalShortcut()
{
#ifdef NOX_HAS_X11
    if (m_display) {
        ungrabShortcut(m_altSpaceKeycode, Mod1Mask);
        ungrabShortcut(m_superNKeycode, Mod4Mask);
        XCloseDisplay(m_display);
    }
#endif
}

bool GlobalShortcut::isAvailable() const
{
    return m_available;
}

QString GlobalShortcut::statusText() const
{
    return m_statusText;
}

void GlobalShortcut::setup()
{
#ifdef NOX_HAS_X11
    if (QGuiApplication::platformName().contains(QStringLiteral("wayland"), Qt::CaseInsensitive)) {
        m_statusText = QStringLiteral("Global shortcuts Super+N and Alt+Space are available on X11 sessions.");
        return;
    }

    m_display = XOpenDisplay(nullptr);
    if (!m_display) {
        m_statusText = QStringLiteral("Unable to bind global shortcuts for NOX.");
        return;
    }

    m_altSpaceKeycode = XKeysymToKeycode(m_display, XK_space);
    m_superNKeycode = XKeysymToKeycode(m_display, XK_n);

    grabShortcut(m_altSpaceKeycode, Mod1Mask);
    grabShortcut(m_superNKeycode, Mod4Mask);
    XFlush(m_display);

    m_notifier = new QSocketNotifier(ConnectionNumber(m_display), QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, [this]() { drainX11Events(); });

    m_available = true;
    m_statusText = QStringLiteral("Press Super+N or Alt+Space to toggle NOX.");
#else
    m_statusText = QStringLiteral("Rebuild with X11 development headers for Super+N and Alt+Space support.");
#endif
}

#ifdef NOX_HAS_X11
void GlobalShortcut::grabShortcut(int keycode, unsigned int modifiers) const
{
    if (!m_display || keycode == 0) {
        return;
    }

    const Window root = DefaultRootWindow(m_display);
    XGrabKey(m_display, keycode, modifiers, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(m_display, keycode, modifiers | Mod2Mask, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(m_display, keycode, modifiers | LockMask, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(m_display, keycode, modifiers | Mod2Mask | LockMask, root, True, GrabModeAsync,
             GrabModeAsync);
}

void GlobalShortcut::ungrabShortcut(int keycode, unsigned int modifiers) const
{
    if (!m_display || keycode == 0) {
        return;
    }

    const Window root = DefaultRootWindow(m_display);
    XUngrabKey(m_display, keycode, modifiers, root);
    XUngrabKey(m_display, keycode, modifiers | Mod2Mask, root);
    XUngrabKey(m_display, keycode, modifiers | LockMask, root);
    XUngrabKey(m_display, keycode, modifiers | Mod2Mask | LockMask, root);
}

void GlobalShortcut::drainX11Events()
{
    while (m_display && XPending(m_display) > 0) {
        XEvent event;
        XNextEvent(m_display, &event);
        if (event.type == KeyPress
            && (event.xkey.keycode == m_altSpaceKeycode || event.xkey.keycode == m_superNKeycode)) {
            emit activated();
        }
    }
}
#endif

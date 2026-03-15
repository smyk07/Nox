#pragma once

#include <QObject>

class QSocketNotifier;

class GlobalShortcut : public QObject
{
    Q_OBJECT

public:
    explicit GlobalShortcut(QObject *parent = nullptr);
    ~GlobalShortcut() override;

    bool isAvailable() const;
    QString statusText() const;

signals:
    void activated();

private:
    void setup();

#ifdef NOX_HAS_X11
    void grabShortcut(int keycode, unsigned int modifiers) const;
    void ungrabShortcut(int keycode, unsigned int modifiers) const;
    void drainX11Events();

    struct _XDisplay *m_display = nullptr;
    QSocketNotifier *m_notifier = nullptr;
    int m_altSpaceKeycode = 0;
    int m_superNKeycode = 0;
#endif

    bool m_available = false;
    QString m_statusText;
};

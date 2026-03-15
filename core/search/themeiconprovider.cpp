#include "core/search/themeiconprovider.h"

#include <QColor>
#include <QIcon>
#include <QPainter>

ThemeIconProvider::ThemeIconProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap ThemeIconProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    const QSize desired = requestedSize.isValid() ? requestedSize : QSize(48, 48);
    const QIcon icon = QIcon::fromTheme(id, QIcon::fromTheme(QStringLiteral("application-x-executable")));
    QPixmap pixmap = icon.pixmap(desired);

    if (pixmap.isNull()) {
        pixmap = QPixmap(desired);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setBrush(QColor(QStringLiteral("#232326")));
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(pixmap.rect(), 12, 12);
        painter.setPen(QColor(QStringLiteral("#9FA1A7")));
        painter.drawText(pixmap.rect(), Qt::AlignCenter, QStringLiteral("•"));
    }

    if (size) {
        *size = pixmap.size();
    }

    return pixmap;
}

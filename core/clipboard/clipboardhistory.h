#pragma once

#include <QObject>
#include <QStringList>

#include "core/search/searchresult.h"

class ClipboardHistory : public QObject
{
    Q_OBJECT

public:
    explicit ClipboardHistory(QObject *parent = nullptr);

    QList<SearchResult> search(const QString &query, int limit = 5) const;
    void copyToClipboard(const QString &text) const;

signals:
    void historyChanged();

private slots:
    void handleClipboardChanged();

private:
    void rememberText(const QString &text);
    QStringList m_entries;
};

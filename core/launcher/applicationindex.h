#pragma once

#include <QObject>
#include <QVector>

#include "core/search/searchresult.h"

struct ApplicationEntry {
    QString name;
    QString comment;
    QString exec;
    QString iconName;
    QString desktopFile;
    QString searchableText;
};

class ApplicationIndex : public QObject
{
    Q_OBJECT

public:
    explicit ApplicationIndex(QObject *parent = nullptr);

    QList<SearchResult> search(const QString &query, int limit = 8) const;
    bool launch(const QString &execLine) const;

signals:
    void indexReady();

private:
    void refreshAsync();
    static QVector<ApplicationEntry> scanApplications();
    static QStringList desktopDirectories();
    static QString normalizeExec(const QString &execLine);

    QVector<ApplicationEntry> m_entries;
};

#pragma once

#include <QObject>
#include <QVector>

#include "core/search/searchresult.h"

struct FileEntry {
    QString name;
    QString path;
    bool isDir = false;
};

class FileIndex : public QObject
{
    Q_OBJECT

public:
    explicit FileIndex(QObject *parent = nullptr);

    QList<SearchResult> search(const QString &query, int limit = 8) const;

signals:
    void indexReady();

private:
    void refreshAsync();
    static QVector<FileEntry> scanFiles();
    static void scanDirectory(const QString &path, int depth, int maxDepth, int &count,
                              QVector<FileEntry> &entries);

    QVector<FileEntry> m_entries;
};

#include "core/files/fileindex.h"

#include <QDir>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QStandardPaths>
#include <QtConcurrent>

#include <algorithm>

#include "core/search/fuzzymatcher.h"

namespace {
constexpr int kMaxIndexedFiles = 2500;
}

FileIndex::FileIndex(QObject *parent)
    : QObject(parent)
{
    refreshAsync();
}

QList<SearchResult> FileIndex::search(const QString &query, int limit) const
{
    QList<SearchResult> results;
    const QString trimmed = query.trimmed();

    if (trimmed.size() < 2 && !trimmed.isEmpty()) {
        return results;
    }

    for (int i = 0; i < m_entries.size(); ++i) {
        const auto &entry = m_entries.at(i);
        int score = 0;

        if (trimmed.isEmpty()) {
            score = 300 - i;
        } else {
            score = FuzzyMatcher::score(trimmed, QStringLiteral("%1 %2").arg(entry.name, entry.path));
            if (score < 0) {
                continue;
            }
        }

        SearchResult result;
        result.id = entry.path;
        result.title = entry.name;
        result.subtitle = entry.path;
        result.iconName = entry.isDir ? QStringLiteral("folder") : QStringLiteral("text-x-generic");
        result.previewTitle = entry.name;
        result.previewText = entry.path;
        result.payload = entry.path;
        result.actions = {entry.isDir ? QStringLiteral("Open folder") : QStringLiteral("Open file")};
        result.kind = SearchResultKind::File;
        result.score = score;
        results.push_back(result);
    }

    std::sort(results.begin(), results.end(), [](const SearchResult &lhs, const SearchResult &rhs) {
        return lhs.score > rhs.score;
    });

    if (results.size() > limit) {
        results = results.mid(0, limit);
    }

    return results;
}

void FileIndex::refreshAsync()
{
    auto *watcher = new QFutureWatcher<QVector<FileEntry>>(this);

    connect(watcher, &QFutureWatcher<QVector<FileEntry>>::finished, this, [this, watcher]() {
        m_entries = watcher->result();
        watcher->deleteLater();
        emit indexReady();
    });

    watcher->setFuture(QtConcurrent::run(&FileIndex::scanFiles));
}

QVector<FileEntry> FileIndex::scanFiles()
{
    QVector<FileEntry> entries;
    int count = 0;

    const QString home = QDir::homePath();
    scanDirectory(home, 0, 2, count, entries);
    scanDirectory(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), 0, 4, count,
                  entries);
    scanDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), 0, 4, count,
                  entries);

    return entries;
}

void FileIndex::scanDirectory(const QString &path, int depth, int maxDepth, int &count,
                              QVector<FileEntry> &entries)
{
    if (path.isEmpty() || count >= kMaxIndexedFiles || depth > maxDepth) {
        return;
    }

    QDir dir(path);
    if (!dir.exists()) {
        return;
    }

    const QFileInfoList items = dir.entryInfoList(
        QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Readable, QDir::DirsFirst | QDir::Name);

    for (const QFileInfo &info : items) {
        if (count >= kMaxIndexedFiles) {
            return;
        }
        if (info.fileName().startsWith(QLatin1Char('.'))) {
            continue;
        }

        FileEntry entry;
        entry.name = info.fileName();
        entry.path = info.absoluteFilePath();
        entry.isDir = info.isDir();
        entries.push_back(entry);
        count++;

        if (info.isDir()) {
            scanDirectory(info.absoluteFilePath(), depth + 1, maxDepth, count, entries);
        }
    }
}

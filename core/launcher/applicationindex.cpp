#include "core/launcher/applicationindex.h"

#include <QDir>
#include <QDirIterator>
#include <QFutureWatcher>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QtConcurrent>

#include <algorithm>

#include "core/search/fuzzymatcher.h"

ApplicationIndex::ApplicationIndex(QObject *parent)
    : QObject(parent)
{
    refreshAsync();
}

QList<SearchResult> ApplicationIndex::search(const QString &query, int limit) const
{
    QList<SearchResult> results;
    const QString trimmed = query.trimmed();

    for (int i = 0; i < m_entries.size(); ++i) {
        const auto &entry = m_entries.at(i);
        int score = 0;

        if (trimmed.isEmpty()) {
            score = 1000 - i;
        } else {
            score = FuzzyMatcher::score(trimmed, entry.searchableText);
            if (score < 0) {
                continue;
            }
        }

        SearchResult result;
        result.id = entry.desktopFile;
        result.title = entry.name;
        result.subtitle = entry.comment.isEmpty() ? normalizeExec(entry.exec) : entry.comment;
        result.iconName = entry.iconName;
        result.previewTitle = entry.name;
        result.previewText =
            QStringLiteral("%1\n%2").arg(entry.desktopFile, normalizeExec(entry.exec));
        result.payload = entry.exec;
        result.actions = {QStringLiteral("Launch application")};
        result.kind = SearchResultKind::Application;
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

bool ApplicationIndex::launch(const QString &execLine) const
{
    const QStringList split = QProcess::splitCommand(execLine);
    QStringList arguments;

    for (const QString &part : split) {
        if (!part.startsWith(QLatin1Char('%'))) {
            arguments << part;
        }
    }

    if (arguments.isEmpty()) {
        return false;
    }

    const QString program = arguments.takeFirst();
    return QProcess::startDetached(program, arguments);
}

void ApplicationIndex::refreshAsync()
{
    auto *watcher = new QFutureWatcher<QVector<ApplicationEntry>>(this);

    connect(watcher, &QFutureWatcher<QVector<ApplicationEntry>>::finished, this, [this, watcher]() {
        m_entries = watcher->result();
        watcher->deleteLater();
        emit indexReady();
    });

    watcher->setFuture(QtConcurrent::run(&ApplicationIndex::scanApplications));
}

QVector<ApplicationEntry> ApplicationIndex::scanApplications()
{
    QVector<ApplicationEntry> entries;
    QStringList seen;

    for (const QString &directory : desktopDirectories()) {
        QDirIterator it(directory, QStringList() << QStringLiteral("*.desktop"), QDir::Files,
                        QDirIterator::Subdirectories);
        while (it.hasNext()) {
            const QString filePath = it.next();
            QSettings desktopFile(filePath, QSettings::IniFormat);
            desktopFile.beginGroup(QStringLiteral("Desktop Entry"));

            if (desktopFile.value(QStringLiteral("Type")).toString() != QStringLiteral("Application")) {
                continue;
            }
            if (desktopFile.value(QStringLiteral("Hidden")).toBool()
                || desktopFile.value(QStringLiteral("NoDisplay")).toBool()) {
                continue;
            }

            ApplicationEntry entry;
            entry.name = desktopFile.value(QStringLiteral("Name")).toString();
            entry.comment = desktopFile.value(QStringLiteral("Comment")).toString();
            entry.exec = desktopFile.value(QStringLiteral("Exec")).toString();
            entry.iconName = desktopFile.value(QStringLiteral("Icon")).toString();
            entry.desktopFile = filePath;

            if (entry.name.isEmpty() || entry.exec.isEmpty() || seen.contains(entry.name)) {
                continue;
            }

            entry.searchableText =
                QStringLiteral("%1 %2 %3").arg(entry.name, entry.comment, normalizeExec(entry.exec));
            entries.push_back(entry);
            seen.push_back(entry.name);
        }
    }

    std::sort(entries.begin(), entries.end(), [](const ApplicationEntry &lhs, const ApplicationEntry &rhs) {
        return lhs.name.toLower() < rhs.name.toLower();
    });

    return entries;
}

QStringList ApplicationIndex::desktopDirectories()
{
    QStringList directories = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
    directories << QStringLiteral("/usr/share/applications");
    directories << QStringLiteral("/usr/local/share/applications");
    directories.removeDuplicates();

    QStringList existing;
    for (const QString &directory : directories) {
        if (QDir(directory).exists()) {
            existing << directory;
        }
    }

    return existing;
}

QString ApplicationIndex::normalizeExec(const QString &execLine)
{
    const QStringList parts = QProcess::splitCommand(execLine);
    QStringList cleaned;

    for (const QString &part : parts) {
        if (!part.startsWith(QLatin1Char('%'))) {
            cleaned << part;
        }
    }

    return cleaned.join(QLatin1Char(' '));
}

#include "plugins/system/systemplugin.h"

#include <QDesktopServices>
#include <QDir>
#include <QProcess>
#include <QStandardPaths>
#include <QUrl>

QString SystemPlugin::pluginId() const
{
    return QStringLiteral("system");
}

QStringList SystemPlugin::registeredCommands() const
{
    return {
        QStringLiteral("shutdown"),
        QStringLiteral("restart"),
        QStringLiteral("open home"),
        QStringLiteral("open downloads"),
        QStringLiteral("open documents"),
        QStringLiteral("kill <process>")
    };
}

QList<SearchResult> SystemPlugin::query(const QString &query) const
{
    Q_UNUSED(query);
    return {};
}

QList<SearchResult> SystemPlugin::commandQuery(const QString &query) const
{
    QList<SearchResult> results;
    const QString trimmed = query.trimmed();

    if (trimmed.isEmpty()) {
        for (const QString &command : registeredCommands()) {
            SearchResult result;
            result.id = QStringLiteral("system:%1").arg(command);
            result.pluginId = pluginId();
            result.title = command;
            result.subtitle = QStringLiteral("System command");
            result.iconName = QStringLiteral("system-run");
            result.previewTitle = QStringLiteral("Command");
            result.previewText = command;
            result.payload = command;
            result.actions = {QStringLiteral("Execute command")};
            result.kind = SearchResultKind::Command;
            result.score = 800;
            results.push_back(result);
        }
        return results;
    }

    const struct CommandDefinition {
        QString match;
        QString title;
        QString subtitle;
        QString preview;
        QString payload;
    } definitions[] = {
        {QStringLiteral("shutdown"), QStringLiteral("Shutdown"),
         QStringLiteral("Power off using systemctl"),
         QStringLiteral("Runs: systemctl poweroff"), QStringLiteral("systemctl poweroff")},
        {QStringLiteral("restart"), QStringLiteral("Restart"),
         QStringLiteral("Reboot using systemctl"), QStringLiteral("Runs: systemctl reboot"),
         QStringLiteral("systemctl reboot")},
        {QStringLiteral("open home"), QStringLiteral("Open Home"),
         QStringLiteral("Reveal your home directory"), QDir::homePath(),
         QStringLiteral("open:%1").arg(QDir::homePath())},
        {QStringLiteral("open downloads"), QStringLiteral("Open Downloads"),
         QStringLiteral("Reveal your downloads folder"),
         QStandardPaths::writableLocation(QStandardPaths::DownloadLocation),
         QStringLiteral("open:%1").arg(
             QStandardPaths::writableLocation(QStandardPaths::DownloadLocation))},
        {QStringLiteral("open documents"), QStringLiteral("Open Documents"),
         QStringLiteral("Reveal your documents folder"),
         QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
         QStringLiteral("open:%1").arg(
             QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation))}
    };

    for (const auto &definition : definitions) {
        if (!definition.match.startsWith(trimmed, Qt::CaseInsensitive)
            && !trimmed.startsWith(definition.match, Qt::CaseInsensitive)) {
            continue;
        }

        SearchResult result;
        result.id = QStringLiteral("system:%1").arg(definition.title);
        result.pluginId = pluginId();
        result.title = definition.title;
        result.subtitle = definition.subtitle;
        result.iconName = QStringLiteral("system-run");
        result.previewTitle = QStringLiteral("Command");
        result.previewText = definition.preview;
        result.payload = definition.payload;
        result.actions = {QStringLiteral("Execute command")};
        result.kind = SearchResultKind::Command;
        result.score = 900;
        results.push_back(result);
    }

    if (trimmed.startsWith(QStringLiteral("kill "), Qt::CaseInsensitive) && trimmed.size() > 5) {
        const QString processName = trimmed.mid(5).trimmed();
        SearchResult result;
        result.id = QStringLiteral("system:kill:%1").arg(processName);
        result.pluginId = pluginId();
        result.title = QStringLiteral("Kill %1").arg(processName);
        result.subtitle = QStringLiteral("pkill -f %1").arg(processName);
        result.iconName = QStringLiteral("process-stop");
        result.previewTitle = QStringLiteral("Command");
        result.previewText = QStringLiteral("Runs: pkill -f %1").arg(processName);
        result.payload = QStringLiteral("pkill -f %1").arg(processName);
        result.actions = {QStringLiteral("Terminate matching process")};
        result.kind = SearchResultKind::Command;
        result.score = 950;
        results.push_back(result);
    }

    SearchResult shellFallback;
    shellFallback.id = QStringLiteral("system:shell:%1").arg(trimmed);
    shellFallback.pluginId = pluginId();
    shellFallback.title = QStringLiteral("Run in shell");
    shellFallback.subtitle = trimmed;
    shellFallback.iconName = QStringLiteral("utilities-terminal");
    shellFallback.previewTitle = QStringLiteral("Shell");
    shellFallback.previewText = trimmed;
    shellFallback.payload = trimmed;
    shellFallback.actions = {QStringLiteral("Execute command in /bin/sh")};
    shellFallback.kind = SearchResultKind::Command;
    shellFallback.score = 700;
    results.push_back(shellFallback);

    return results;
}

bool SystemPlugin::execute(const SearchResult &result)
{
    if (result.payload.startsWith(QStringLiteral("open:"))) {
        return QDesktopServices::openUrl(QUrl::fromLocalFile(result.payload.mid(5)));
    }

    return QProcess::startDetached(QStringLiteral("/bin/sh"),
                                   {QStringLiteral("-lc"), result.payload});
}

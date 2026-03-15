#include "core/clipboard/clipboardhistory.h"

#include <QClipboard>
#include <QGuiApplication>

#include "core/search/fuzzymatcher.h"

namespace {
constexpr int kMaxClipboardEntries = 30;
}

ClipboardHistory::ClipboardHistory(QObject *parent)
    : QObject(parent)
{
    auto *clipboard = QGuiApplication::clipboard();
    connect(clipboard, &QClipboard::dataChanged, this, &ClipboardHistory::handleClipboardChanged);
    rememberText(clipboard->text(QClipboard::Clipboard));
}

QList<SearchResult> ClipboardHistory::search(const QString &query, int limit) const
{
    QList<SearchResult> results;
    const QString trimmed = query.trimmed();

    for (int i = 0; i < m_entries.size(); ++i) {
        const QString &entry = m_entries.at(i);
        int score = 0;

        if (trimmed.isEmpty()) {
            score = 500 - i;
        } else {
            score = FuzzyMatcher::score(trimmed, entry);
            if (score < 0) {
                continue;
            }
        }

        const QStringList lines = entry.split(QLatin1Char('\n'));
        SearchResult result;
        result.id = QStringLiteral("clipboard:%1").arg(i);
        result.title = lines.value(0).left(48);
        result.subtitle = entry.left(90).replace(QLatin1Char('\n'), QStringLiteral(" "));
        result.iconName = QStringLiteral("edit-paste");
        result.previewTitle = QStringLiteral("Clipboard");
        result.previewText = entry;
        result.payload = entry;
        result.actions = {QStringLiteral("Copy back to clipboard")};
        result.kind = SearchResultKind::Clipboard;
        result.score = score;
        results.push_back(result);
    }

    if (results.size() > limit) {
        results = results.mid(0, limit);
    }

    return results;
}

void ClipboardHistory::copyToClipboard(const QString &text) const
{
    auto *clipboard = QGuiApplication::clipboard();
    clipboard->setText(text, QClipboard::Clipboard);
}

void ClipboardHistory::handleClipboardChanged()
{
    rememberText(QGuiApplication::clipboard()->text(QClipboard::Clipboard));
}

void ClipboardHistory::rememberText(const QString &text)
{
    const QString trimmed = text.trimmed();
    if (trimmed.isEmpty()) {
        return;
    }

    m_entries.removeAll(trimmed);
    m_entries.prepend(trimmed);

    while (m_entries.size() > kMaxClipboardEntries) {
        m_entries.removeLast();
    }

    emit historyChanged();
}

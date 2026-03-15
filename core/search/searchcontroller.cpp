#include "core/search/searchcontroller.h"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QTimer>
#include <QUrl>

#include <algorithm>

SearchController::SearchController(QObject *parent)
    : QObject(parent)
{
    connect(&m_applicationIndex, &ApplicationIndex::indexReady, this, &SearchController::refreshResults);
    connect(&m_clipboardHistory, &ClipboardHistory::historyChanged, this,
            &SearchController::refreshResults);
    connect(&m_fileIndex, &FileIndex::indexReady, this, &SearchController::refreshResults);
    connect(&m_globalShortcut, &GlobalShortcut::activated, this, &SearchController::toggleVisible);

    QTimer::singleShot(0, this, &SearchController::refreshResults);
    emit statusTextChanged();
}

SearchModel *SearchController::model()
{
    return &m_model;
}

int SearchController::currentIndex() const
{
    return m_currentIndex;
}

QString SearchController::previewTitle() const
{
    return currentResult().previewTitle;
}

QString SearchController::previewText() const
{
    return currentResult().previewText;
}

QStringList SearchController::currentActions() const
{
    return currentResult().actions;
}

bool SearchController::isVisible() const
{
    return m_visible;
}

bool SearchController::actionMenuVisible() const
{
    return m_actionMenuVisible;
}

QString SearchController::statusText() const
{
    return m_globalShortcut.statusText();
}

bool SearchController::hotkeyAvailable() const
{
    return m_globalShortcut.isAvailable();
}

void SearchController::setQuery(const QString &query)
{
    if (m_query == query) {
        return;
    }

    m_query = query;
    refreshResults();
}

void SearchController::setCurrentIndex(int index)
{
    const int bounded = std::clamp(index, 0, std::max(0, m_model.count() - 1));
    if (bounded == m_currentIndex) {
        return;
    }

    m_currentIndex = bounded;
    emit currentIndexChanged();
    emit selectionChanged();
}

void SearchController::moveSelection(int delta)
{
    if (m_model.count() == 0) {
        return;
    }

    int next = m_currentIndex + delta;
    if (next < 0) {
        next = m_model.count() - 1;
    } else if (next >= m_model.count()) {
        next = 0;
    }

    setCurrentIndex(next);
}

void SearchController::activateSelection()
{
    const SearchResult result = currentResult();
    if (result.title.isEmpty()) {
        return;
    }

    if (executeResult(result)) {
        closeLauncher();
    }
}

void SearchController::toggleActionMenu()
{
    m_actionMenuVisible = !m_actionMenuVisible;
    emit actionMenuVisibleChanged();
}

void SearchController::closeLauncher()
{
    if (!m_visible) {
        return;
    }

    m_visible = false;
    m_actionMenuVisible = false;
    emit actionMenuVisibleChanged();
    emit visibleChanged();
}

void SearchController::quitApplication()
{
    QCoreApplication::quit();
}

void SearchController::showLauncher()
{
    if (m_visible) {
        return;
    }

    m_visible = true;
    m_query.clear();
    refreshResults();
    emit visibleChanged();
}

void SearchController::toggleVisible()
{
    if (m_visible) {
        closeLauncher();
    } else {
        showLauncher();
    }
}

void SearchController::refreshResults()
{
    QList<SearchResult> results;
    const QString trimmed = m_query.trimmed();

    if (trimmed.startsWith(QLatin1Char('>'))) {
        results = m_pluginManager.commandQuery(trimmed.mid(1));
    } else {
        results.append(m_pluginManager.query(trimmed));
        results.append(m_applicationIndex.search(trimmed, trimmed.isEmpty() ? 7 : 8));
        results.append(m_clipboardHistory.search(trimmed, trimmed.isEmpty() ? 3 : 5));
        if (!trimmed.isEmpty()) {
            results.append(m_fileIndex.search(trimmed, 6));
        }
    }

    std::sort(results.begin(), results.end(), [](const SearchResult &lhs, const SearchResult &rhs) {
        return lhs.score > rhs.score;
    });

    if (results.size() > 12) {
        results = results.mid(0, 12);
    }

    m_model.setResults(results);

    const int nextIndex =
        results.isEmpty() ? 0 : std::min(m_currentIndex, static_cast<int>(results.size()) - 1);
    if (nextIndex != m_currentIndex) {
        m_currentIndex = nextIndex;
        emit currentIndexChanged();
    }

    emit selectionChanged();
}

SearchResult SearchController::currentResult() const
{
    return m_model.resultAt(m_currentIndex);
}

bool SearchController::executeResult(const SearchResult &result)
{
    switch (result.kind) {
    case SearchResultKind::Application:
        return m_applicationIndex.launch(result.payload);
    case SearchResultKind::Clipboard:
        m_clipboardHistory.copyToClipboard(result.payload);
        return true;
    case SearchResultKind::File:
        return QDesktopServices::openUrl(QUrl::fromLocalFile(result.payload));
    case SearchResultKind::Calculator:
    case SearchResultKind::Command:
    case SearchResultKind::Plugin:
        return m_pluginManager.execute(result);
    }

    return false;
}

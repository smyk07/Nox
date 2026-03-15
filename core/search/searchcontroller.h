#pragma once

#include <QObject>
#include <QStringList>

#include "core/clipboard/clipboardhistory.h"
#include "core/files/fileindex.h"
#include "core/hotkey/globalshortcut.h"
#include "core/launcher/applicationindex.h"
#include "core/plugin/pluginmanager.h"
#include "core/search/searchmodel.h"

class SearchController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(SearchModel *model READ model CONSTANT)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QString previewTitle READ previewTitle NOTIFY selectionChanged)
    Q_PROPERTY(QString previewText READ previewText NOTIFY selectionChanged)
    Q_PROPERTY(QStringList currentActions READ currentActions NOTIFY selectionChanged)
    Q_PROPERTY(bool visible READ isVisible NOTIFY visibleChanged)
    Q_PROPERTY(bool actionMenuVisible READ actionMenuVisible NOTIFY actionMenuVisibleChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(bool hotkeyAvailable READ hotkeyAvailable NOTIFY statusTextChanged)

public:
    explicit SearchController(QObject *parent = nullptr);

    SearchModel *model();
    int currentIndex() const;
    QString previewTitle() const;
    QString previewText() const;
    QStringList currentActions() const;
    bool isVisible() const;
    bool actionMenuVisible() const;
    QString statusText() const;
    bool hotkeyAvailable() const;

    Q_INVOKABLE void setQuery(const QString &query);
    Q_INVOKABLE void setCurrentIndex(int index);
    Q_INVOKABLE void moveSelection(int delta);
    Q_INVOKABLE void activateSelection();
    Q_INVOKABLE void toggleActionMenu();
    Q_INVOKABLE void closeLauncher();
    Q_INVOKABLE void quitApplication();
    Q_INVOKABLE void showLauncher();
    Q_INVOKABLE void toggleVisible();

signals:
    void currentIndexChanged();
    void selectionChanged();
    void visibleChanged();
    void actionMenuVisibleChanged();
    void statusTextChanged();

private:
    void refreshResults();
    SearchResult currentResult() const;
    bool executeResult(const SearchResult &result);

    SearchModel m_model;
    ApplicationIndex m_applicationIndex;
    PluginManager m_pluginManager;
    ClipboardHistory m_clipboardHistory;
    FileIndex m_fileIndex;
    GlobalShortcut m_globalShortcut;
    QString m_query;
    int m_currentIndex = 0;
    bool m_visible = true;
    bool m_actionMenuVisible = false;
};

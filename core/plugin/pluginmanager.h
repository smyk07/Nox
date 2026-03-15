#pragma once

#include <QObject>
#include <QVector>

#include "core/plugin/searchplugin.h"

class PluginManager : public QObject
{
    Q_OBJECT

public:
    explicit PluginManager(QObject *parent = nullptr);

    QList<SearchResult> query(const QString &query) const;
    QList<SearchResult> commandQuery(const QString &query) const;
    bool execute(const SearchResult &result);

private:
    QVector<SearchPlugin *> m_plugins;
};

#pragma once

#include "core/plugin/searchplugin.h"

class SystemPlugin : public SearchPlugin
{
    Q_OBJECT

public:
    using SearchPlugin::SearchPlugin;

    QString pluginId() const override;
    QStringList registeredCommands() const override;
    QList<SearchResult> query(const QString &query) const override;
    QList<SearchResult> commandQuery(const QString &query) const override;
    bool execute(const SearchResult &result) override;
};

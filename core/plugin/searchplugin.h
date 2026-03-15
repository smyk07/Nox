#pragma once

#include <QObject>

#include "core/search/searchresult.h"

class SearchPlugin : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;
    ~SearchPlugin() override = default;

    virtual QString pluginId() const = 0;
    virtual QStringList registeredCommands() const { return {}; }
    virtual QList<SearchResult> query(const QString &query) const = 0;
    virtual QList<SearchResult> commandQuery(const QString &query) const { return {}; }
    virtual bool execute(const SearchResult &result) = 0;
};

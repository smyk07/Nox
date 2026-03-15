#pragma once

#include "core/plugin/searchplugin.h"

class CalculatorPlugin : public SearchPlugin
{
    Q_OBJECT

public:
    using SearchPlugin::SearchPlugin;

    QString pluginId() const override;
    QList<SearchResult> query(const QString &query) const override;
    bool execute(const SearchResult &result) override;

private:
    static bool isMathExpression(const QString &query);
    static QString evaluateExpression(const QString &query);
};

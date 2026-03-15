#include "core/plugin/pluginmanager.h"

#include "plugins/calculator/calculatorplugin.h"
#include "plugins/system/systemplugin.h"

PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
{
    m_plugins.push_back(new CalculatorPlugin(this));
    m_plugins.push_back(new SystemPlugin(this));
}

QList<SearchResult> PluginManager::query(const QString &query) const
{
    QList<SearchResult> results;
    for (const auto *plugin : m_plugins) {
        results.append(plugin->query(query));
    }
    return results;
}

QList<SearchResult> PluginManager::commandQuery(const QString &query) const
{
    QList<SearchResult> results;
    for (const auto *plugin : m_plugins) {
        results.append(plugin->commandQuery(query));
    }
    return results;
}

bool PluginManager::execute(const SearchResult &result)
{
    for (auto *plugin : m_plugins) {
        if (plugin->pluginId() == result.pluginId) {
            return plugin->execute(result);
        }
    }
    return false;
}

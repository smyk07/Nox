#include "plugins/calculator/calculatorplugin.h"

#include <QGuiApplication>
#include <QClipboard>
#include <QJSEngine>
#include <QRegularExpression>

#include <cmath>

QString CalculatorPlugin::pluginId() const
{
    return QStringLiteral("calculator");
}

QList<SearchResult> CalculatorPlugin::query(const QString &query) const
{
    QList<SearchResult> results;
    const QString trimmed = query.trimmed();

    if (!isMathExpression(trimmed)) {
        return results;
    }

    const QString value = evaluateExpression(trimmed);
    if (value.isEmpty()) {
        return results;
    }

    SearchResult result;
    result.id = QStringLiteral("calculator:%1").arg(trimmed);
    result.pluginId = pluginId();
    result.title = value;
    result.subtitle = trimmed;
    result.iconName = QStringLiteral("accessories-calculator");
    result.previewTitle = QStringLiteral("Calculator");
    result.previewText = QStringLiteral("%1 = %2").arg(trimmed, value);
    result.payload = value;
    result.actions = {QStringLiteral("Copy result to clipboard")};
    result.kind = SearchResultKind::Calculator;
    result.score = 10'000;
    results.push_back(result);

    return results;
}

bool CalculatorPlugin::execute(const SearchResult &result)
{
    QGuiApplication::clipboard()->setText(result.payload, QClipboard::Clipboard);
    return true;
}

bool CalculatorPlugin::isMathExpression(const QString &query)
{
    if (query.isEmpty()) {
        return false;
    }

    static const QRegularExpression allowed(
        QStringLiteral(R"(^[0-9\.\+\-\*\/%\(\)\s]+$)"));
    return allowed.match(query).hasMatch();
}

QString CalculatorPlugin::evaluateExpression(const QString &query)
{
    QJSEngine engine;
    const QJSValue value = engine.evaluate(query);
    if (value.isError() || !value.isNumber()) {
        return {};
    }

    const double number = value.toNumber();
    if (!std::isfinite(number)) {
        return {};
    }

    return QString::number(number, 'g', 12);
}

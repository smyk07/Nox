#pragma once

#include <QString>
#include <QStringList>

enum class SearchResultKind {
    Application,
    Command,
    Calculator,
    Clipboard,
    File,
    Plugin
};

struct SearchResult {
    QString id;
    QString pluginId;
    QString title;
    QString subtitle;
    QString iconName;
    QString previewTitle;
    QString previewText;
    QString payload;
    QStringList actions;
    SearchResultKind kind = SearchResultKind::Plugin;
    int score = 0;
};

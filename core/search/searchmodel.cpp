#include "core/search/searchmodel.h"

SearchModel::SearchModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int SearchModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_results.size();
}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_results.size()) {
        return {};
    }

    const SearchResult &result = m_results.at(index.row());

    switch (role) {
    case TitleRole:
        return result.title;
    case SubtitleRole:
        return result.subtitle;
    case IconNameRole:
        return result.iconName;
    case PreviewTitleRole:
        return result.previewTitle;
    case PreviewTextRole:
        return result.previewText;
    case KindRole:
        return static_cast<int>(result.kind);
    default:
        return {};
    }
}

QHash<int, QByteArray> SearchModel::roleNames() const
{
    return {
        {TitleRole, "title"},
        {SubtitleRole, "subtitle"},
        {IconNameRole, "iconName"},
        {PreviewTitleRole, "previewTitle"},
        {PreviewTextRole, "previewText"},
        {KindRole, "kind"}
    };
}

void SearchModel::setResults(const QList<SearchResult> &results)
{
    beginResetModel();
    m_results = results;
    endResetModel();
}

SearchResult SearchModel::resultAt(int index) const
{
    if (index < 0 || index >= m_results.size()) {
        return {};
    }

    return m_results.at(index);
}

int SearchModel::count() const
{
    return m_results.size();
}

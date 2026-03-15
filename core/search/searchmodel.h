#pragma once

#include <QAbstractListModel>
#include <QList>

#include "core/search/searchresult.h"

class SearchModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        SubtitleRole,
        IconNameRole,
        PreviewTitleRole,
        PreviewTextRole,
        KindRole
    };

    explicit SearchModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setResults(const QList<SearchResult> &results);
    SearchResult resultAt(int index) const;
    int count() const;

private:
    QList<SearchResult> m_results;
};

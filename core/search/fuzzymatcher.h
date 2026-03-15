#pragma once

#include <QString>

class FuzzyMatcher
{
public:
    static int score(const QString &query, const QString &candidate);
};

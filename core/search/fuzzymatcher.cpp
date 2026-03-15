#include "core/search/fuzzymatcher.h"

#include <QChar>

int FuzzyMatcher::score(const QString &query, const QString &candidate)
{
    const QString q = query.trimmed().toLower();
    const QString c = candidate.toLower();

    if (q.isEmpty()) {
        return 0;
    }

    int total = 0;
    int consecutive = 0;
    int cursor = 0;

    for (int i = 0; i < q.size(); ++i) {
        const QChar needle = q.at(i);
        bool found = false;

        while (cursor < c.size()) {
            if (c.at(cursor) == needle) {
                const bool atBoundary =
                    cursor == 0 || !c.at(cursor - 1).isLetterOrNumber();
                total += atBoundary ? 12 : 6;
                total += consecutive * 4;
                consecutive++;
                found = true;
                cursor++;
                break;
            }

            total -= 1;
            consecutive = 0;
            cursor++;
        }

        if (!found) {
            return -1;
        }
    }

    total += q.size() * 8;
    total -= q.size() > c.size() ? 0 : (c.size() - q.size()) / 3;
    return total;
}

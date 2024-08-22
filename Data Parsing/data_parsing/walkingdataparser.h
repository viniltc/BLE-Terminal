#ifndef WALKINGDATAPARSER_H
#define WALKINGDATAPARSER_H

#include <QString>
#include "walkingdata.h"
#include <optional>

class WalkingDataParser {
public:
    std::optional<WalkingData> parse(const QString &fileName);

private:
    std::optional<QDate> parseDate(const QString &line);
    std::optional<QTime> parseTime(const QString &line);
    std::optional<int> parseInt(const QString &line);
    std::optional<float> parseFloat(const QString &line);

    void logError(const QString &message);
};

#endif // WALKINGDATAPARSER_H

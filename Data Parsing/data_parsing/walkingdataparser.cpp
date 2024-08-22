#include "walkingdataparser.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>
#include <optional>

std::optional<WalkingData> WalkingDataParser::parse(const QString &fileName) {
    WalkingData data;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        logError("Failed to open file");
        return std::nullopt;
    }

    QTextStream in(&file);
    QString line;

    while (!in.atEnd()) {
        line = in.readLine().trimmed();

        if (line.startsWith("Start Date:")) {
            auto result = parseDate(line);
            if (result) data.startDate = *result;
        } else if (line.startsWith("End Date:")) {
            auto result = parseDate(line);
            if (result) data.endDate = *result;
        } else if (line.startsWith("Total Number of Days:")) {
            auto result = parseInt(line);
            if (result) data.totalDays = *result;
        } else if (line.startsWith("Total Number of Days Used:")) {
            auto result = parseInt(line);
            if (result) data.daysUsed = *result;
        } else if (line.startsWith("Average Days Used per Week:")) {
            auto result = parseFloat(line);
            if (result) data.avgDaysPerWeek = *result;
        } else if (line.startsWith("CH1 Total Steps:")) {
            auto result = parseInt(line);
            if (result) data.ch1TotalSteps = *result;
        } else if (line.startsWith("CH2 Total Steps:")) {
            auto result = parseInt(line);
            if (result) data.ch2TotalSteps = *result;
        } else if (line.startsWith("Steps per Active Day:")) {
            auto result = parseInt(line);
            if (result) data.stepsPerActiveDay = *result;
        } else if (line.startsWith("CH1 Average Steps per Day:")) {
            auto result = parseInt(line);
            if (result) data.ch1AvgStepsPerDay = *result;
        } else if (line.startsWith("CH2 Average Steps per Day:")) {
            auto result = parseInt(line);
            if (result) data.ch2AvgStepsPerDay = *result;
        } else if (line.startsWith("Total Walks Initiated:")) {
            auto result = parseInt(line);
            if (result) data.totalWalksInitiated = *result;
        } else if (line.startsWith("Walks per Active Day:")) {
            auto result = parseFloat(line);
            if (result) data.walksPerActiveDay = *result;
        } else if (line.startsWith("Average Pauses per Active Day:")) {
            auto result = parseFloat(line);
            if (result) data.avgPausesPerActiveDay = *result;
        } else if (line.startsWith("Total Walk Time:")) {
            auto result = parseTime(line);
            if (result) data.totalWalkTime = *result;
        } else if (line.startsWith("Average Walk Time per Active Day:")) {
            auto result = parseTime(line);
            if (result) data.avgWalkTimePerActiveDay = *result;
        } else if (line.startsWith("Total Stim Time CH1:")) {
            auto result = parseTime(line);
            if (result) data.totalStimTimeCh1 = *result;
        } else if (line.startsWith("Total Stim Time CH2:")) {
            auto result = parseTime(line);
            if (result) data.totalStimTimeCh2 = *result;
        } else if (line.startsWith("Average Dose per Active Day CH1:")) {
            auto result = parseTime(line);
            if (result) data.avgDosePerActiveDayCh1 = *result;
        } else if (line.startsWith("Average Dose per Active Day CH2:")) {
            auto result = parseTime(line);
            if (result) data.avgDosePerActiveDayCh2 = *result;
        } else if (line.startsWith("High Activity Periods:")) {
            auto result = parseInt(line);
            if (result) data.highActivityPeriods = *result;
        } else if (line.startsWith("Medium Activity Periods:")) {
            auto result = parseInt(line);
            if (result) data.mediumActivityPeriods = *result;
        } else if (line.startsWith("Low Activity Periods:")) {
            auto result = parseInt(line);
            if (result) data.lowActivityPeriods = *result;
        } else if (line.startsWith("Steps in High Activity:")) {
            auto result = parseInt(line);
            if (result) data.stepsInHighActivity = *result;
        } else if (line.startsWith("Steps in Medium Activity:")) {
            auto result = parseInt(line);
            if (result) data.stepsInMediumActivity = *result;
        } else if (line.startsWith("Steps in Low Activity:")) {
            auto result = parseInt(line);
            if (result) data.stepsInLowActivity = *result;
        } else if (line.startsWith("High Activity Time:")) {
            auto result = parseTime(line);
            if (result) data.highActivityTime = *result;
        } else if (line.startsWith("Medium Activity Time:")) {
            auto result = parseTime(line);
            if (result) data.mediumActivityTime = *result;
        } else if (line.startsWith("Low Activity Time:")) {
            auto result = parseTime(line);
            if (result) data.lowActivityTime = *result;
        }
    }

    file.close();
    return data;
}

std::optional<QDate> WalkingDataParser::parseDate(const QString &line) {
    QStringList parts = line.split(": ");
    if (parts.size() < 2) {
        logError("Failed to parse date: " + line);
        return std::nullopt;
    }
    return QDate::fromString(parts.last(), "yyyy-MM-dd");
}

std::optional<QTime> WalkingDataParser::parseTime(const QString &line) {
    QStringList parts = line.split(": ");
        if (parts.size() < 2) {
            logError("Failed to parse time: " + line);
            return std::nullopt;
        }

        QString timeString = parts.last();
        QStringList timeParts = timeString.split("h ");
        int hours = 0;
        int minutes = 0;

        if (timeParts.size() >= 1) {
            hours = timeParts[0].toInt();  // Parse hours
        }

        if (timeParts.size() == 2) {
            minutes = timeParts[1].replace("m", "").toInt();  // Parse minutes if present
        }

        return QTime(hours, minutes);
}

std::optional<int> WalkingDataParser::parseInt(const QString &line) {
    QStringList parts = line.split(": ");
    if (parts.size() < 2) {
        logError("Failed to parse int: " + line);
        return std::nullopt;
    }
    bool ok;
    int value = parts.last().toInt(&ok);
    if (!ok) {
        logError("Invalid integer value: " + line);
        return std::nullopt;
    }
    return value;
}

std::optional<float> WalkingDataParser::parseFloat(const QString &line) {
    QStringList parts = line.split(": ");
    if (parts.size() < 2) {
        logError("Failed to parse float: " + line);
        return std::nullopt;
    }
    bool ok;
    float value = parts.last().split(" ").first().toFloat(&ok);
    if (!ok) {
        logError("Invalid float value: " + line);
        return std::nullopt;
    }
    return value;
}

void WalkingDataParser::logError(const QString &message) {
    qDebug() << "Error:" << message;
}

#ifndef WALKINGDATA_H
#define WALKINGDATA_H

#include <QDate>
#include <QTime>

struct WalkingData {
    QDate startDate;
    QDate endDate;
    int totalDays;

    int daysUsed;
    float avgDaysPerWeek;

    int ch1TotalSteps;
    int ch2TotalSteps;
    int stepsPerActiveDay;
    int ch1AvgStepsPerDay;
    int ch2AvgStepsPerDay;

    int totalWalksInitiated;
    float walksPerActiveDay;
    float avgPausesPerActiveDay;

    QTime totalWalkTime;
    QTime avgWalkTimePerActiveDay;

    QTime totalStimTimeCh1;
    QTime totalStimTimeCh2;
    QTime avgDosePerActiveDayCh1;
    QTime avgDosePerActiveDayCh2;

    int highActivityPeriods;
    int mediumActivityPeriods;
    int lowActivityPeriods;

    int stepsInHighActivity;
    int stepsInMediumActivity;
    int stepsInLowActivity;

    QTime highActivityTime;
    QTime mediumActivityTime;
    QTime lowActivityTime;
};

#endif // WALKINGDATA_H

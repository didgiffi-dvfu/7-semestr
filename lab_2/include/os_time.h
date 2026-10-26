#ifndef OS_TIME_H
#define OS_TIME_H

#include <string>
#include <cstdio>
using namespace std;

struct TimePoint
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int millis;
};

TimePoint date_time_now();

unsigned long current_pid();

inline string date_time_formatter(const TimePoint &t)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d.%03d", t.year, t.month, t.day, t.hour, t.minute, t.second, t.millis);
    return string(buf);
};
#endif
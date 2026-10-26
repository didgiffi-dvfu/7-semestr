#include <windows.h>
#include "os_time.h"

TimePoint date_time_now()
{

    SYSTEMTIME st;
    GetLocalTime(&st);

    TimePoint time_val = TimePoint();

    time_val.year = st.wYear;
    time_val.month = st.wMonth;
    time_val.day = st.wDay;
    time_val.hour = st.wHour;
    time_val.minute = st.wMinute;
    time_val.second = st.wSecond;
    time_val.millis = st.wMilliseconds;

    return time_val;
}

unsigned long current_pid()
{
    return (unsigned long)GetCurrentProcessId();
};

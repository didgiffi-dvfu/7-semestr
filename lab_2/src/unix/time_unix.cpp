#include "os_time.h"
#include <sys/time.h> // gettimeofday
#include <time.h>      //localtime_r
#include <unistd.h>    // getpid

TimePoint date_time_now() {
 struct timeval time_val;

    gettimeofday(&time_val, nullptr);
    
    struct tm local_time;
    // ruskladivaet sec po polyam
    localtime_r(&time_val.tv_sec, &local_time);
    
    TimePoint t = TimePoint();

    t.year = local_time.tm_year + 1900;
    t.month = local_time.tm_mon+1;
    t.day = local_time.tm_mday;
    t.hour = local_time.tm_hour;
    t.minute = local_time.tm_min;
    t.second = local_time.tm_sec;
    t.millis = time_val.tv_usec/1000;

    return  t;

};

unsigned long current_pid(){
    return (unsigned long)getpid();
};

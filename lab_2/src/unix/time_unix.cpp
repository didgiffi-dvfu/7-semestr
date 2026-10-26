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

    t.year = local_time.tm_year;
    t.month =
    

    


};

unsigned long current_pid(){
    return (unsigned long)getpid();
};

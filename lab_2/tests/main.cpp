#include <iostream>
#include <chrono>
#include <ostream>
#include <thread>
#include "logger.h"
#include "os_time.h"

using namespace std;

int main()
{
    cout << "Time: " << date_time_formatter(date_time_now()) << endl;
    cout << "PID: " << current_pid() << endl;

    Logger log;
    if (!log.open("/tmp/lab2.log")) {
        cerr << "Log file is not open"<< endl;

        return 1;
    }
    log.log("start");
    log.log("counter=42");
    log.close();
    cout<< " lines in lab2.log"<<endl;
    return 0;
}
#include <iostream>
#include <chrono>
#include <thread>
#include "process_runner.h"
#include "os_time.h"

using namespace std;

int main()
{
    cout << "Time: " << date_time_formatter(date_time_now()) << endl;
    cout << "PID: " << current_pid() << endl;
    return 0;
}
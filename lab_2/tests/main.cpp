#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <string>

#include "os_time.h"
#include "logger.h"
#include "shared_counter.h"

using namespace std;

static const char *COUNTER_NAME = "lab2_counter";

void ticker_300ms(SharedCounter &counter, atomic<bool> &stop)
{
    while (!stop.load())
    {
        this_thread::sleep_for(chrono::milliseconds(300));
        counter.add(1);
    }
}

void logger_1s(SharedCounter &counter, Logger &log, atomic<bool> &stop)
{
    while (!stop.load())
    {
        this_thread::sleep_for(chrono::seconds(1));
        log.log("counter= " + to_string(counter.get()));
    }
}

int main()
{
    Logger log;
    if (!log.open("lab_2.log"))
    {
        cerr << "Log open failed" << endl;
        return 1;
    }

    SharedCounter counter;
    if (!counter.open(COUNTER_NAME))
    {
        cerr << "Shared counter open failed" << endl;
        return 1;
    }

    log.log("START");

    atomic<bool> stop{false};
    thread t1(ticker_300ms, ref(counter), ref(stop));
    thread t2(logger_1s, ref(counter), ref(log), ref(stop));

    cout << "Enter a number to set the counter, q to exit" << endl;
    string line;
    while (getline(cin, line))
    {
        if (line == "q" || line == "quit")
            break;

        try
        {
            long long v = stoll(line);
            counter.set(v);
            cout << "Counter = " << v << endl;
        }
        catch (...)
        {
            cout << "Enter a number or q" << endl;
        }
    }

    stop.store(true);
    t1.join();
    t2.join();

    log.log("EXIT");
    counter.close();
    return 0;
}

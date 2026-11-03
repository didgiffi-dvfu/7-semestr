#include <iostream>
#include <chrono>
#include <ostream>
#include <thread>
// переменная которую можно безопастно читать и писать из разных потоков
#include <atomic>
#include "os_time.h"
#include "counter.h"
#include "logger.h"

using namespace std;

// void timer_loop(Counter &counter, int interval_ms, atomic<bool> &stop)
// { // атомарное чтение
//     while (!stop.load())
//     {
//         this_thread::sleep_for(chrono::milliseconds(interval_ms));
//         counter.increment();
//         cout << "Counter value: " << counter.get() << endl;
//     }
// }

void ticker_300ms(Counter &counter, atomic<bool> &stop)
{
    while (!stop.load())
    {
        this_thread::sleep_for(chrono::milliseconds(300));
        counter.increment();
        
    }
}

void logger_1s(Counter &counter, Logger &log, atomic<bool> &stop)
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
        cerr << "Log is not oper" << endl;
        return 1;
    }

    log.log("START");
    Counter counter;
    atomic<bool> stop{false};

    thread t1(ticker_300ms, ref(counter), ref(stop));
    thread t2(logger_1s, ref(counter), ref(log), ref(stop));

    cout << "Wright number for counter, wridht \"q\" for exit " << endl;
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
            cout << "Wright q or num" << endl;
        }
    }

    stop.store(true);
    t1.join();
    t2.join();

    log.log("EXIT");
    return 0;
}

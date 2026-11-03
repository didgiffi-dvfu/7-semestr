#include <iostream>
#include <chrono>
#include <ostream>
#include <thread>
// переменная которую можно безопастно читать и писать из разных потоков
#include <atomic>

#include "process_runner.h"
#include "os_time.h"
#include "counter.h"

using namespace std;

void timer_loop(Counter &counter, int interval_ms, atomic<bool> &stop)
{ // атомарное чтение
    while (!stop.load())
    {
        this_thread::sleep_for(chrono::milliseconds(interval_ms));
        counter.increment();
        cout << "Counter value: " << counter.get() << endl;
    }
}

int main()
{
    Counter counter;
    atomic<bool> stop{false};

    thread timer_thread(timer_loop, ref(counter), 300, ref(stop));

    this_thread::sleep_for(chrono::seconds(3));
    // атомарная запись
    stop.store(true);
    timer_thread.join();

    cout << "Final counter value: " << counter.get() << endl;
    return 0;
}
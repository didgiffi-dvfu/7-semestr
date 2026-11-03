#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <string>

#include "os_time.h"
#include "logger.h"
#include "shared_counter.h"
#include "child_process.h"

using namespace std;

static const char *COUNTER_NAME = "lab2_counter";

static void interruptible_sleep(int total_ms, std::atomic<bool> &stop)
{
    const int step_ms = 50;
    for (int slept = 0; slept < total_ms && !stop.load(); slept += step_ms)
        std::this_thread::sleep_for(std::chrono::milliseconds(step_ms));
}


// Копия 1: увеличить счётчик на 10 (пункт 5a)
void run_copy1(SharedCounter &counter, Logger &log)
{
    log.log("COPY1 START");
    counter.add(10);
    log.log("COPY1 EXIT");
}

// Копия 2: ×2, через 2 секунды ÷2 (пункт 5b)
void run_copy2(SharedCounter &counter, Logger &log)
{
    log.log("COPY2 START");
    counter.multiply(2);
    this_thread::sleep_for(chrono::seconds(2));
    counter.divide(2);
    log.log("COPY2 EXIT");
}


// Нить 1: +1 каждые 300 мс (пункт 2) — это делают ВСЕ экземпляры.
void ticker_300ms(SharedCounter &counter, atomic<bool> &stop)
{
    while (!stop.load())
    {
        interruptible_sleep(300, stop);
        if (stop.load()) break;
        counter.add(1);
    }
}

void logger_1s(SharedCounter &counter, Logger &log, long long my_pid, atomic<bool> &stop)
{
    while (!stop.load())
    {
        interruptible_sleep(1000, stop);
        if (stop.load()) break;
        if (counter.try_become_leader(my_pid))   
        {
            log.log("counter= " + to_string(counter.get()));
        }
    }
}


void spawner_3s(SharedCounter &counter, Logger &log, const string &exe,
                long long my_pid, atomic<bool> &stop)
{
    ChildProcess c1; c1.valid = false;   
    ChildProcess c2; c2.valid = false;   

    while (!stop.load())
    {
        interruptible_sleep(3000, stop);
        if (stop.load()) break;

        if (!counter.try_become_leader(my_pid))
        {
            continue;   
        }

        // --- копия 1 ---
        if (c1.valid && is_running(c1))
        {
            log.log("copy 1 still running, skip");   
        }
        else
        {
            close_child(c1);
            c1 = spawn_copy(exe, "1");
        }

        // --- копия 2 ---
        if (c2.valid && is_running(c2))
        {
            log.log("copy 2 still running, skip");   
        }
        else
        {
            close_child(c2);
            c2 = spawn_copy(exe, "2");
        }
    }

    close_child(c1);
    close_child(c2);
}

int main(int argc, char **argv)
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

    if (argc >= 3 && string(argv[1]) == "--child")
    {
        string which = argv[2];
        if (which == "1")
            run_copy1(counter, log);
        else if (which == "2")
            run_copy2(counter, log);

        counter.close();
        return 0;  
    }

    long long my_pid = (long long)current_pid();
    counter.try_become_leader(my_pid); 
    log.log("START");

    atomic<bool> stop{false};
    thread t1(ticker_300ms, ref(counter), ref(stop));
    thread t2(logger_1s, ref(counter), ref(log), my_pid, ref(stop));
    thread t3(spawner_3s, ref(counter), ref(log), string(argv[0]), my_pid, ref(stop));

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
    t3.join();

    log.log("EXIT");
    counter.close();
    return 0;
}

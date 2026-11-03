#ifndef SHARED_COUNTER_H
#define SHARED_COUNTER_H

#include <string>

#if defined(_WIN32) || defined(_WIN64)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN   
#endif
#include <windows.h>
#else
#include <semaphore.h>        // sem_t — именованный семафор POSIX
#endif

// То, что физически лежит в разделяемой памяти (одно на всех).
struct SharedData
{
    long long counter;
    long long leader_pid;   // PID лидера; 0 = лидера нет
};

class SharedCounter
{
public:
    SharedCounter();
    ~SharedCounter();

    // Создать/открыть общий счётчик и лок по ИМЕНИ (одинаковому у всех процессов).
    bool open(const std::string &name);
    void close();
    bool is_open() const { return open_; }


    
    void add(long long delta)  { if (!open_) return; lock(); data_->counter += delta; unlock(); }
    void multiply(long long f) { if (!open_) return; lock(); data_->counter *= f;     unlock(); }
    void divide(long long f)   { if (!open_) return; lock(); if (f != 0) data_->counter /= f; unlock(); }
    void set(long long value)  { if (!open_) return; lock(); data_->counter = value;  unlock(); }

    
    long long get()
    {
        if (!open_) return 0;
        lock();
        long long v = data_->counter;
        unlock();
        return v;
    }

    
    bool try_become_leader(long long my_pid);

private:
    void lock();    
    
    void unlock(); 
    

    SharedData *data_; 
    bool open_;

#if defined(_WIN32) || defined(_WIN64)
    HANDLE map_handle_;    // область памяти (CreateFileMapping)
    HANDLE mutex_handle_;  // именованный мьютекс-лок
#else
    int    shm_fd_;        // дескриптор разделяемой памяти (shm_open)
    sem_t *sem_;           // именованный семафор-лок (sem_open)
#endif

    // Копировать объект нельзя
    SharedCounter(const SharedCounter &) = delete;
    SharedCounter &operator=(const SharedCounter &) = delete;
};

#endif 

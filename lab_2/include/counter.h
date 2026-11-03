#ifndef COUNTER_H
#define COUNTER_H

#include <mutex>

class Counter
{
public:
    void increment()
    {
        //  lk ее консткуртор захватывает мьютекс когда выполнение доходит
        // до конца функции то переменнная автоматически уничтожается и ее деструктор отпускает mutex_

        std::lock_guard<std::mutex> lk(mutex_);
        value_ += 1;
    }

    void set(long long v)
    {
        std::lock_guard<std::mutex> lk(mutex_);

        value_ = v;
    }

    long long get()
    {
        std::lock_guard<std::mutex> lk(mutex_);

        return value_;
    }

private:
    long long value_ = 0;
    std::mutex mutex_;
};
#endif

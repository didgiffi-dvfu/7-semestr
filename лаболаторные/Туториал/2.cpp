// Лабораторная 2. OpenMP. Суммирование элементов массива.
// Показаны 4 способа суммировать массив в параллельной области:
//   1) НЕПРАВИЛЬНО — общая переменная sum без защиты (гонка данных)
//   2) reduction(+:sum)
//   3) локальная сумма + atomic
//   4) локальная сумма + critical
// Массив заполнен единицами, поэтому правильная сумма = n.
//
// Компиляция (Linux): g++ -o 2.o 2.cpp -fopenmp

#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

int main() {
    const int n = 1000000;      // размер массива
    vector<int> a(n, 1);        // все элементы = 1  => правильная сумма = n
    long long sum;

    // ---------- 1) НЕПРАВИЛЬНО: общая sum без синхронизации ----------
    sum = 0;
#pragma omp parallel
    {
#pragma omp for
        for (int i = 0; i < n; ++i)
            sum += a[i];        // несколько потоков пишут в одну sum одновременно -> гонка
    }
    cout << "1) Общая sum (без защиты) = " << sum << "  (правильно: " << n << ")\n";
    // Почему неверно и нестабильно: операция sum += a[i] — это ЧТЕНИЕ, сложение и ЗАПИСЬ.
    // Два потока могут прочитать одно и то же старое значение и затереть прибавку друг друга.
    // Поэтому при разных запусках результат разный и обычно меньше n.

    // ---------- 2) ПРАВИЛЬНО: reduction(+:sum) ----------
    sum = 0;
#pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; ++i)
        sum += a[i];
    // У каждого потока своя локальная копия sum; в конце OpenMP сам их складывает.
    cout << "2) reduction(+:sum)       = " << sum << "\n";

    // ---------- 3) локальная сумма + atomic ----------
    sum = 0;
#pragma omp parallel
    {
        long long local_sum = 0;        // своя сумма у каждого потока
#pragma omp for
        for (int i = 0; i < n; ++i)
            local_sum += a[i];          // без конфликтов — переменная локальная
#pragma omp atomic
        sum += local_sum;               // одна атомарная операция на поток
    }
    cout << "3) atomic                 = " << sum << "\n";

    // ---------- 4) локальная сумма + critical ----------
    sum = 0;
#pragma omp parallel
    {
        long long local_sum = 0;
#pragma omp for
        for (int i = 0; i < n; ++i)
            local_sum += a[i];
#pragma omp critical            // в этот блок входит только один поток за раз
        {
            sum += local_sum;
        }
    }
    cout << "4) critical               = " << sum << "\n";

    return 0;
}

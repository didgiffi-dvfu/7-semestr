// Лабораторная 7. OpenMP. Замки (locks).
// Вариант №1: суммирование элементов массива (10 000 элементов) с защитой
// общей переменной суммы замком omp_set_lock / omp_unset_lock.
// (Номер варианта уточните у преподавателя; здесь показан принцип работы замка.)
//
// Компиляция (Linux): g++ -o 7.o 7.cpp -fopenmp

#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

int main() {
    const int n = 10000;
    vector<int> a(n, 1);         // 10 000 единиц => правильная сумма = 10 000
    long long sum = 0;

    omp_lock_t lock;
    omp_init_lock(&lock);        // 1) создаём замок

#pragma omp parallel num_threads(4)
    {
        // Каждая нить копит свою часть локально — это быстрее,
        // а замок используем лишь для редкого обновления общей суммы.
        long long local = 0;
#pragma omp for
        for (int i = 0; i < n; ++i)
            local += a[i];

        omp_set_lock(&lock);     // 2) захватываем замок (ждём, если занят)
        sum += local;            //    критическая операция над общей переменной
        omp_unset_lock(&lock);   // 3) освобождаем замок
    }

    omp_destroy_lock(&lock);     // 4) уничтожаем замок

    cout << "Сумма = " << sum << " (правильно: " << n << ")\n";
    return 0;
}

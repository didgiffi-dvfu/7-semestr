// Лабораторная 4. OpenMP. Скалярное произведение двух векторов.
// Скалярное произведение: dot = a[0]*b[0] + a[1]*b[1] + ... + a[n-1]*b[n-1].
// Складываем произведения через reduction(+:dot). Замеряем время для 1..10 потоков.
//
// Компиляция (Linux): g++ -o 4.o 4.cpp -fopenmp

#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

int main() {
    // ---- Проверка на 10 элементах ----
    // a = 1..10, b = 1..10 => dot = 1*1+2*2+...+10*10 = 385
    {
        double a[10], b[10], dot = 0;
        for (int i = 0; i < 10; ++i) { a[i] = i + 1; b[i] = i + 1; }
#pragma omp parallel for reduction(+:dot)
        for (int i = 0; i < 10; ++i)
            dot += a[i] * b[i];
        cout << "Проверка на 10 элементах: dot = " << dot << " (ожидаем 385)\n\n";
    }

    // ---- Большие векторы для замеров ----
    const int n = 20000000;
    vector<double> a(n), b(n);
    for (int i = 0; i < n; ++i) { a[i] = 1.0; b[i] = 2.0; }  // dot = 2*n

    cout << "Потоки\tВремя, с\tdot\n";
    for (int threads = 1; threads <= 10; ++threads) {
        double dot = 0.0;
        double t0 = omp_get_wtime();
#pragma omp parallel for num_threads(threads) reduction(+:dot)
        for (int i = 0; i < n; ++i)
            dot += a[i] * b[i];
        double t1 = omp_get_wtime();

        cout << threads << "\t" << (t1 - t0) << "\t" << dot << "\n";
    }
    return 0;
}

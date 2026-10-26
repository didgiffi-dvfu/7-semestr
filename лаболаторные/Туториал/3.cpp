// Лабораторная 3. OpenMP. Поиск максимума и минимума вектора.
// Каждый поток ищет свой локальный min/max, доступ к общим min/max защищён
// через reduction(min:...) и reduction(max:...).
// Замеряем время (omp_get_wtime) для 1..10 потоков на большом векторе.
//
// Компиляция (Linux): g++ -o 3.o 3.cpp -fopenmp

#include <iostream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <omp.h>
using namespace std;

int main() {
    // ---- Сначала проверка правильности на 10 элементах ----
    int small[10] = {4, -2, 17, 5, 0, 23, -9, 8, 11, 3};
    int smax = INT_MIN, smin = INT_MAX;
#pragma omp parallel for reduction(max:smax) reduction(min:smin)
    for (int i = 0; i < 10; ++i) {
        if (small[i] > smax) smax = small[i];
        if (small[i] < smin) smin = small[i];
    }
    cout << "Проверка на 10 элементах: max = " << smax
         << ", min = " << smin << " (ожидаем max=23, min=-9)\n\n";

    // ---- Большой вектор для замеров времени ----
    const int n = 20000000;      // 20 млн элементов
    vector<int> v(n);
    for (int i = 0; i < n; ++i)
        v[i] = rand();           // заполняем случайными числами

    cout << "Потоки\tВремя, с\tmax\t\tmin\n";
    for (int threads = 1; threads <= 10; ++threads) {
        int vmax = INT_MIN, vmin = INT_MAX;

        double t0 = omp_get_wtime();
#pragma omp parallel for num_threads(threads) reduction(max:vmax) reduction(min:vmin)
        for (int i = 0; i < n; ++i) {
            if (v[i] > vmax) vmax = v[i];
            if (v[i] < vmin) vmin = v[i];
        }
        double t1 = omp_get_wtime();

        cout << threads << "\t" << (t1 - t0) << "\t"
             << vmax << "\t" << vmin << "\n";
    }
    return 0;
}

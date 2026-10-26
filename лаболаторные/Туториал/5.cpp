// Лабораторная 5. OpenMP. Число "пи".
// Считаем pi как интеграл от 4/(1+x^2) на [0,1] методом прямоугольников:
//   pi ~= (1/N) * sum_{i=0..N-1} 4 / (1 + x_i^2),  где x_i = (i + 0.5)/N
// Сумму распределяем по нитям через reduction(+:sum).
// Сравниваем результат с M_PI и сравниваем время 1 нить / максимум нитей.
//
// Компиляция (Linux): g++ -o 5.o 5.cpp -fopenmp

#include <iostream>
#include <iomanip>
#include <cmath>     // M_PI
#include <omp.h>
using namespace std;

// Один общий расчёт pi на заданном числе нитей
double calc_pi(long long N, int threads) {
    double sum = 0.0;
    double step = 1.0 / (double)N;
#pragma omp parallel for num_threads(threads) reduction(+:sum)
    for (long long i = 0; i < N; ++i) {
        double x = (i + 0.5) * step;   // середина i-го прямоугольника
        sum += 4.0 / (1.0 + x * x);
    }
    return sum * step;                 // умножаем на ширину 1/N
}

int main() {
    long long N = 1000000000;          // точность; чем больше N, тем точнее
    int maxth = omp_get_max_threads();

    // --- 1 нить ---
    double t0 = omp_get_wtime();
    double pi1 = calc_pi(N, 1);
    double t1 = omp_get_wtime();

    // --- максимум нитей ---
    double t2 = omp_get_wtime();
    double piM = calc_pi(N, maxth);
    double t3 = omp_get_wtime();

    cout << setprecision(10) << fixed;
    cout << "N = " << N << "\n\n";
    cout << "1 нить:        pi = " << pi1 << "   время = " << (t1 - t0) << " c\n";
    cout << maxth << " нитей:      pi = " << piM << "   время = " << (t3 - t2) << " c\n";
    cout << "M_PI (cmath):  pi = " << M_PI << "\n";
    cout << "Погрешность (макс. нитей) = " << fabs(piM - M_PI) << "\n";
    if (t3 - t2 > 0)
        cout << "Ускорение = " << (t1 - t0) / (t3 - t2) << "x\n";
    return 0;
}

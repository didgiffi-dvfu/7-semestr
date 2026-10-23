#include <cmath>
#include <iomanip>
#include <iostream>
#include <omp.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct PiResult {
    double value;
    int actual_threads;
    double seconds;
};

PiResult calculate_pi(long long intervals, int requested_threads) {
    const double step = 1.0 / static_cast<double>(intervals);
    double sum = 0.0;
    int actual_threads = 1;

    const double start = omp_get_wtime();
#pragma omp parallel num_threads(requested_threads) reduction(+:sum)
    {
#pragma omp master
        actual_threads = omp_get_num_threads();

#pragma omp for schedule(static)
        for (long long i = 0; i < intervals; ++i) {
            const double x = (static_cast<double>(i) + 0.5) * step;
            sum += 4.0 / (1.0 + x * x);
        }
    }
    const double finish = omp_get_wtime();

    return {sum * step, actual_threads, finish - start};
}

int main() {
    long long n = 0;
    std::cout << "Enter N: ";
    if (!(std::cin >> n) || n <= 0) {
        std::cerr << "N must be a positive integer.\n";
        return 1;
    }

    omp_set_dynamic(0);
    const int maximum_threads = omp_get_max_threads();

    const PiResult one_thread = calculate_pi(n, 1);
    const PiResult many_threads = calculate_pi(n, maximum_threads);

    std::cout << std::fixed << std::setprecision(15);
    std::cout << "M_PI:             " << M_PI << "\n";
    std::cout << "Pi, 1 thread:     " << one_thread.value << "\n";
    std::cout << "Absolute error:   "
              << std::abs(one_thread.value - M_PI) << "\n";
    std::cout << "Time, 1 thread:   " << one_thread.seconds << " s\n\n";

    std::cout << "Pi, " << many_threads.actual_threads << " threads: "
              << many_threads.value << "\n";
    std::cout << "Absolute error:   "
              << std::abs(many_threads.value - M_PI) << "\n";
    std::cout << "Time, " << many_threads.actual_threads << " threads: "
              << many_threads.seconds << " s\n";
    std::cout << "Speedup:          "
              << one_thread.seconds / many_threads.seconds << "\n";

    return 0;
}

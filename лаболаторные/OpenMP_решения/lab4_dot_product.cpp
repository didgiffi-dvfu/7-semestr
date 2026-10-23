#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>
#include <omp.h>

struct DotResult {
    double value;
    int actual_threads;
    double seconds;
};

DotResult dot_product(const std::vector<double>& a,
                      const std::vector<double>& b,
                      int requested_threads) {
    double result = 0.0;
    int actual_threads = 1;
    const long long n = static_cast<long long>(a.size());

    const double start = omp_get_wtime();
#pragma omp parallel num_threads(requested_threads) reduction(+:result)
    {
#pragma omp master
        actual_threads = omp_get_num_threads();

#pragma omp for schedule(static)
        for (long long i = 0; i < n; ++i) {
            const std::size_t index = static_cast<std::size_t>(i);
            result += a[index] * b[index];
        }
    }
    const double finish = omp_get_wtime();

    return {result, actual_threads, finish - start};
}

int main() {
    omp_set_dynamic(0);

    const std::vector<double> check_a = {1, 2, 3, 4, 5,
                                         6, 7, 8, 9, 10};
    const std::vector<double> check_b = {10, 9, 8, 7, 6,
                                         5, 4, 3, 2, 1};
    double expected = 0.0;
    for (std::size_t i = 0; i < check_a.size(); ++i) {
        expected += check_a[i] * check_b[i];
    }
    const DotResult check = dot_product(check_a, check_b, 4);

    std::cout << "Check on 10 elements\n";
    std::cout << "Expected dot product: " << expected << "\n";
    std::cout << "OpenMP dot product:   " << check.value << "\n\n";

    const std::size_t n = 10'000'000;
    std::vector<double> a(n);
    std::vector<double> b(n);
    for (std::size_t i = 0; i < n; ++i) {
        a[i] = 1.0 + static_cast<double>(i % 100) / 100.0;
        b[i] = 2.0 - static_cast<double>(i % 80) / 100.0;
    }

    std::cout << "Benchmark vector size: " << n << "\n";
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "threads\ttime, s\tspeedup\tdot product\n";

    double one_thread_time = 0.0;
    for (int threads = 1; threads <= 10; ++threads) {
        DotResult best{};
        best.seconds = std::numeric_limits<double>::max();

        for (int repeat = 0; repeat < 3; ++repeat) {
            const DotResult current = dot_product(a, b, threads);
            if (current.seconds < best.seconds) {
                best = current;
            }
        }

        if (threads == 1) {
            one_thread_time = best.seconds;
        }
        const double speedup = one_thread_time / best.seconds;
        std::cout << best.actual_threads << '\t' << best.seconds << '\t'
                  << speedup << '\t' << best.value << '\n';
    }

    return std::abs(check.value - expected) < 1e-12 ? 0 : 1;
}

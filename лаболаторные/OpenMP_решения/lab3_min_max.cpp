#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>
#include <omp.h>

struct MinMaxResult {
    int minimum;
    int maximum;
    int actual_threads;
    double seconds;
};

MinMaxResult find_min_max(const std::vector<int>& values,
                          int requested_threads) {
    int minimum = std::numeric_limits<int>::max();
    int maximum = std::numeric_limits<int>::lowest();
    int actual_threads = 1;
    const long long n = static_cast<long long>(values.size());

    const double start = omp_get_wtime();
#pragma omp parallel num_threads(requested_threads) \
    reduction(min:minimum) reduction(max:maximum)
    {
#pragma omp master
        actual_threads = omp_get_num_threads();

#pragma omp for schedule(static)
        for (long long i = 0; i < n; ++i) {
            const int value = values[static_cast<std::size_t>(i)];
            minimum = std::min(minimum, value);
            maximum = std::max(maximum, value);
        }
    }
    const double finish = omp_get_wtime();

    return {minimum, maximum, actual_threads, finish - start};
}

int main() {
    omp_set_dynamic(0);

    const std::vector<int> check_values = {8, -4, 15, 0, 3,
                                           42, -11, 7, 9, 1};
    const auto [sequential_min, sequential_max] =
        std::minmax_element(check_values.begin(), check_values.end());
    const MinMaxResult check = find_min_max(check_values, 4);

    std::cout << "Check on 10 elements\n";
    std::cout << "Expected min/max: " << *sequential_min << " / "
              << *sequential_max << "\n";
    std::cout << "OpenMP min/max:   " << check.minimum << " / "
              << check.maximum << "\n\n";

    const std::size_t n = 20'000'000;
    std::vector<int> values(n);
    for (std::size_t i = 0; i < n; ++i) {
        const long long generated =
            (static_cast<long long>(i) * 48'271LL) % 2'000'001LL;
        values[i] = static_cast<int>(generated - 1'000'000LL);
    }

    std::cout << "Benchmark vector size: " << n << "\n";
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "threads\ttime, s\tspeedup\tmin\tmax\n";

    double one_thread_time = 0.0;
    for (int threads = 1; threads <= 10; ++threads) {
        MinMaxResult best{};
        best.seconds = std::numeric_limits<double>::max();

        for (int repeat = 0; repeat < 3; ++repeat) {
            const MinMaxResult current = find_min_max(values, threads);
            if (current.seconds < best.seconds) {
                best = current;
            }
        }

        if (threads == 1) {
            one_thread_time = best.seconds;
        }
        const double speedup = one_thread_time / best.seconds;
        std::cout << best.actual_threads << '\t' << best.seconds << '\t'
                  << speedup << '\t' << best.minimum << '\t'
                  << best.maximum << '\n';
    }

    return (check.minimum == *sequential_min &&
            check.maximum == *sequential_max) ? 0 : 1;
}

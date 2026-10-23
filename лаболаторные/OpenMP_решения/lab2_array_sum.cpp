#include <cstddef>
#include <iostream>
#include <vector>
#include <omp.h>

int main() {
    const std::size_t n = 1'000'000;
    const std::vector<int> a(n, 1);
    const long long expected = static_cast<long long>(n);

    std::cout << "Array size: " << n << "\n";
    std::cout << "Expected sum: " << expected << "\n\n";

    // Учебно ошибочный вариант: несколько потоков одновременно меняют sum.
    for (int attempt = 1; attempt <= 5; ++attempt) {
        long long sum = 0;
#pragma omp parallel for shared(sum)
        for (std::size_t i = 0; i < n; ++i) {
            sum += a[i];
        }
        std::cout << "Race attempt " << attempt << ": " << sum << "\n";
    }

    long long sum = 0;
#pragma omp parallel for reduction(+:sum)
    for (std::size_t i = 0; i < n; ++i) {
        sum += a[i];
    }
    std::cout << "\nReduction sum: " << sum << "\n";

    sum = 0;
#pragma omp parallel
    {
        long long local_sum = 0;
#pragma omp for
        for (std::size_t i = 0; i < n; ++i) {
            local_sum += a[i];
        }
#pragma omp atomic
        sum += local_sum;
    }
    std::cout << "Atomic sum: " << sum << "\n";

    sum = 0;
#pragma omp parallel
    {
        long long local_sum = 0;
#pragma omp for
        for (std::size_t i = 0; i < n; ++i) {
            local_sum += a[i];
        }
#pragma omp critical(sum_update)
        {
            sum += local_sum;
        }
    }
    std::cout << "Critical sum: " << sum << "\n";

    return sum == expected ? 0 : 1;
}

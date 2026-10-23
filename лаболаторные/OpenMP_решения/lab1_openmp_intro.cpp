#include <cstdio>
#include <omp.h>

int main() {
#ifdef _OPENMP
    std::printf("OpenMP is supported. _OPENMP = %d\n", _OPENMP);
#else
    std::printf("OpenMP is not enabled during compilation.\n");
    return 1;
#endif

    // Запрещаем среде выполнения самовольно уменьшать число потоков.
    omp_set_dynamic(0);

    std::printf("\n1) Parallel region with the default thread count\n");
#pragma omp parallel
    {
        const int thread_id = omp_get_thread_num();
        std::printf("Thread id: %d\n", thread_id);

#pragma omp master
        {
            std::printf("The master thread reports %d threads in total.\n",
                        omp_get_num_threads());
        }
    }

    // Значение по умолчанию для следующих parallel-областей.
    omp_set_num_threads(2);

    std::printf("\n2) Region with num_threads(3)\n");
#pragma omp parallel num_threads(3)
    {
        std::printf("Region 1, thread %d of %d\n",
                    omp_get_thread_num(), omp_get_num_threads());
    }

    std::printf("\n3) Region using omp_set_num_threads(2)\n");
#pragma omp parallel
    {
        std::printf("Region 2, thread %d of %d\n",
                    omp_get_thread_num(), omp_get_num_threads());
    }

    return 0;
}

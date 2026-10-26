// Лабораторная 1. OpenMP. Введение.
// Компиляция (Linux):   g++ -o 1.o 1.cpp -fopenmp
// Компиляция (macOS):   clang++ -Xpreprocessor -fopenmp 1.cpp -o 1.o \
//                        -I/opt/homebrew/opt/libomp/include \
//                        -L/opt/homebrew/opt/libomp/lib -lomp
// Запуск: ./1.o

#include <cstdio>
#include <omp.h>   // функции OpenMP: omp_get_thread_num() и т.д.

int main() {
    // --- Задание 1: проверка поддержки OpenMP (пример 1a) ---
    // Макрос _OPENMP определён, только если компилятор собрал программу с OpenMP.
#ifdef _OPENMP
    printf("OpenMP поддерживается! Версия (_OPENMP) = %d\n", _OPENMP);
#else
    printf("OpenMP НЕ поддерживается.\n");
#endif

    printf("\n--- Задание 2: номера потоков и master ---\n");
    // Параллельная область: один и тот же блок выполнят несколько потоков.
#pragma omp parallel
    {
        int num   = omp_get_thread_num();    // мой номер (0..N-1)
        int count = omp_get_num_threads();   // сколько всего потоков

        // Каждый поток печатает свой идентификатор
        printf("Привет от потока номер %d\n", num);

        // Общее количество потоков печатает ТОЛЬКО master (поток 0)
#pragma omp master
        printf(">>> Master: всего потоков = %d\n", count);
    }

    printf("\n--- Задание 3: omp_set_num_threads + num_threads (пример 5a) ---\n");
    // Задаём число потоков по умолчанию = 2
    omp_set_num_threads(2);

    // Но для ЭТОЙ области опция num_threads(3) перебивает значение -> будет 3 потока
#pragma omp parallel num_threads(3)
    {
        printf("Область 1 (num_threads(3)): поток %d\n", omp_get_thread_num());
    }

    // Здесь num_threads не указан -> действует omp_set_num_threads(2) -> 2 потока
#pragma omp parallel
    {
        printf("Область 2 (по умолчанию = 2): поток %d\n", omp_get_thread_num());
    }

    // Параллельных областей в программе может быть сколько угодно.
    return 0;
}

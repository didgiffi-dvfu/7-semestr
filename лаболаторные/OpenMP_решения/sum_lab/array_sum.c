/* Лабораторная работа: суммирование элементов массива в OpenMP.
   Массив заполнен единицами, поэтому правильная сумма = размеру массива N.
   Демонстрируются четыре способа суммирования в параллельной области:
     1) omp for без защиты (гонка данных — результат неверный);
     2) omp for с опцией reduction(+: sum);
     3) omp for + директива atomic (на основе примера 27a);
     4) omp for + директива critical (на основе примера 26a).
   Перед каждой областью, где используется sum, значение обнуляется. */
#include <stdio.h>
#include <omp.h>

int main(int argc, char *argv[])
{
    const int n = 1000000;        /* размер массива (любой) */
    static int a[1000000];
    int i;
    long long sum;

    /* Заполняем массив единицами для проверки правильности суммы. */
    for (i = 0; i < n; i++)
        a[i] = 1;

    printf("Размер массива: %d\n", n);
    printf("Правильная сумма: %d\n\n", n);

    /* ---------- 1. omp for БЕЗ reduction (гонка данных) ----------
       Несколько нитей одновременно читают и пишут общую sum,
       часть прибавлений теряется, поэтому результат меньше n
       и меняется от запуска к запуску. */
    sum = 0;
#pragma omp parallel shared(a, sum)
    {
#pragma omp for
        for (i = 0; i < n; i++)
            sum += a[i];
    }
    printf("1) Без защиты (гонка):    sum = %lld\n", sum);

    /* ---------- 2. omp for с reduction(+: sum) ----------
       У каждой нити своя копия sum, в конце они складываются.
       Результат всегда правильный. */
    sum = 0;
#pragma omp parallel shared(a) reduction(+: sum)
    {
#pragma omp for
        for (i = 0; i < n; i++)
            sum += a[i];
    }
    printf("2) reduction(+: sum):     sum = %lld\n", sum);

    /* ---------- 3. omp for + atomic (пример 27a) ----------
       Каждая нить копит частичную сумму в local_sum,
       а к общей sum прибавляет её атомарно (без прерываний). */
    sum = 0;
#pragma omp parallel shared(a, sum)
    {
        long long local_sum = 0;
#pragma omp for
        for (i = 0; i < n; i++)
            local_sum += a[i];
#pragma omp atomic
        sum += local_sum;
    }
    printf("3) atomic:                sum = %lld\n", sum);

    /* ---------- 4. omp for + critical (пример 26a) ----------
       Критическая секция выполняется только одной нитью за раз,
       поэтому прибавление local_sum к sum безопасно. */
    sum = 0;
#pragma omp parallel shared(a, sum)
    {
        long long local_sum = 0;
#pragma omp for
        for (i = 0; i < n; i++)
            local_sum += a[i];
#pragma omp critical
        {
            sum += local_sum;
        }
    }
    printf("4) critical:              sum = %lld\n", sum);

    return 0;
}

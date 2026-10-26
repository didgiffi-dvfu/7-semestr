/* Пример 18a. Директива for и распределение итераций между нитями.
   Массивы A, B, C общие (shared), счётчик i и номер нити n приватные.
   Директива for делит итерации цикла между нитями. */
#include <stdio.h>
#include <omp.h>

int main(int argc, char *argv[])
{
    int A[10], B[10], C[10], i, n;

    /* Заполним исходные массивы */
    for (i = 0; i < 10; i++) { A[i] = i; B[i] = 2 * i; C[i] = 0; }

#pragma omp parallel shared(A, B, C) private(i, n)
    {
        /* Получим номер текущей нити */
        n = omp_get_thread_num();
#pragma omp for
        for (i = 0; i < 10; i++)
        {
            C[i] = A[i] + B[i];
            printf("Нить %d сложила элементы с номером %d\n", n, i);
        }
    }
    return 0;
}

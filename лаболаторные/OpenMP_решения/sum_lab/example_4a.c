/* Пример 4a. Опция reduction на языке Си.
   Каждая нить получает свою копию count, инициализированную нулём.
   После завершения параллельной области все копии складываются (+). */
#include <stdio.h>

int main(int argc, char *argv[])
{
    int count = 0;
#pragma omp parallel reduction(+: count)
    {
        count++;
        printf("Текущее значение count: %d\n", count);
    }
    printf("Число нитей: %d\n", count);
    return 0;
}

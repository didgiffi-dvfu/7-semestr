# Суммирование массива в OpenMP

Файлы:
- `example_4a.c` — пример 4a (опция `reduction`);
- `example_18a.c` — пример 18a (директива `for`, распределение итераций);
- `array_sum.c` — основное задание: 4 способа суммирования (гонка, reduction, atomic, critical).

## Сборка (macOS, clang + libomp)

```bash
L=/opt/homebrew/opt/libomp
clang array_sum.c -Xpreprocessor -fopenmp -I$L/include -L$L/lib -lomp -o array_sum
OMP_NUM_THREADS=4 ./array_sum
```

На Linux с gcc достаточно: `gcc -fopenmp array_sum.c -o array_sum`.

## Почему без reduction сумма неверна

Переменная `sum` общая для всех нитей. Операция `sum += a[i]` не атомарна:
нить читает старое значение `sum`, прибавляет элемент и записывает обратно.
Если две нити читают `sum` одновременно, одна запись затирает другую —
часть прибавлений теряется. Поэтому результат меньше `N` и меняется от запуска
к запуску. `reduction`, `atomic` и `critical` устраняют гонку и дают верную сумму.

"""Лабораторная №1, вариант 18: минимум 4x²/(3+x²)."""
from dataclasses import dataclass
from math import ceil, sqrt, isfinite
from pathlib import Path
import csv

OUT = Path(__file__).resolve().parent


def f(x):
    return 4 * x**2 / (3 + x**2)


@dataclass
class Result:
    x: float
    value: float
    iterations: int
    calls: int
    resolution: float


class Counter:
    def __init__(self, func):
        self.func, self.calls = func, 0

    def __call__(self, x):
        self.calls += 1
        return self.func(x)


def validate(a, b, eps):
    if not all(isfinite(v) for v in (a, b, eps)) or a >= b or eps <= 0:
        raise ValueError('Нужны конечные a < b и eps > 0')


def dichotomy_method(func, a, b, eps=1e-5):
    validate(a, b, eps)
    counted = Counter(func)
    delta = eps / 10
    iterations = 0
    while b - a > eps:
        mid = (a + b) / 2
        x1, x2 = mid - delta, mid + delta
        if counted(x1) <= counted(x2):
            b = x2
        else:
            a = x1
        iterations += 1
    x = (a + b) / 2
    value = counted(x)
    return Result(x, value, iterations, counted.calls, b - a)


def golden_section_method(func, a, b, eps=1e-5):
    validate(a, b, eps)
    counted = Counter(func)
    tau = (sqrt(5) - 1) / 2
    iterations = 0
    if b - a > eps:
        x1, x2 = b - tau * (b - a), a + tau * (b - a)
        f1, f2 = counted(x1), counted(x2)
        while b - a > eps:
            if f1 <= f2:
                b, x2, f2 = x2, x1, f1
                if b - a > eps:
                    x1 = b - tau * (b - a)
                    f1 = counted(x1)
            else:
                a, x1, f1 = x1, x2, f2
                if b - a > eps:
                    x2 = a + tau * (b - a)
                    f2 = counted(x2)
            iterations += 1
    x = (a + b) / 2
    value = counted(x)
    return Result(x, value, iterations, counted.calls, b - a)


def exhaustive_search(func, a, b, eps=1e-5):
    validate(a, b, eps)
    counted = Counter(func)
    n = ceil((b - a) / eps)
    step = (b - a) / n
    best_x, best_value = a, counted(a)
    for i in range(1, n + 1):
        x = b if i == n else a + i * step
        value = counted(x)
        if value < best_value:
            best_x, best_value = x, value
    return Result(best_x, best_value, n, counted.calls, step)


def make_plots(results):
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt
    import numpy as np

    x = np.linspace(-10, 10, 2001)
    fig, axes = plt.subplots(1, 2, figsize=(12, 4.8))
    for ax in axes:
        ax.plot(x, f(x), label=r'$f(x)=4x^2/(3+x^2)$')
        ax.scatter([0], [0], color='red', zorder=5, label='Минимум (0; 0)')
        ax.grid(alpha=.3)
        ax.set_xlabel('x')
        ax.set_ylabel('f(x)')
    axes[0].axhline(4, linestyle='--', color='gray', label='Асимптота y = 4')
    axes[0].scatter([-1, 1], [1, 1], color='green', label='Точки перегиба')
    axes[0].set_title('Вариант 18: график функции')
    axes[1].set_xlim(-2, 3)
    axes[1].set_ylim(-.15, 3.2)
    axes[1].set_title('Отрезок поиска [−2; 3]')
    for ax in axes:
        ax.legend()
    fig.tight_layout()
    fig.savefig(OUT / 'function_plot.png', dpi=160)
    plt.close(fig)

    fig, ax = plt.subplots(figsize=(8, 4.8))
    bars = ax.bar(list(results), [r.calls for r in results.values()], color=['#4c78a8', '#59a14f', '#f28e2b'])
    ax.set_yscale('log')
    ax.set_ylabel('Число вычислений f(x), логарифмическая шкала')
    ax.set_title('Сравнение методов при ε = 10⁻⁵')
    ax.bar_label(bars, padding=4)
    ax.set_ylim(1, 2000000)
    fig.tight_layout()
    fig.savefig(OUT / 'comparison.png', dpi=160)
    plt.close(fig)


def main():
    methods = {'Дихотомия': dichotomy_method,
               'Золотое сечение': golden_section_method,
               'Перебор': exhaustive_search}
    results = {name: method(f, -2., 3., 1e-5) for name, method in methods.items()}
    header = '| Метод | x̂ | f(x̂) | Итерации / шаги | Вызовы f | Ошибка по x | Длина / шаг |'
    lines = [header, '|---|---:|---:|---:|---:|---:|---:|']
    for name, r in results.items():
        lines.append(f'| {name} | {r.x:.10g} | {r.value:.10g} | {r.iterations} | {r.calls} | {abs(r.x):.10g} | {r.resolution:.10g} |')
    table = '\n'.join(lines)
    (OUT / 'results.md').write_text('# Результаты: вариант 18\n\nОтрезок [−2; 3], ε = 10⁻⁵. Точный минимум: x* = 0, f(x*) = 0.\n\n' + table + '\n', encoding='utf-8')
    with (OUT / 'results.csv').open('w', newline='', encoding='utf-8') as stream:
        writer = csv.writer(stream)
        writer.writerow(['method', 'x', 'f(x)', 'iterations', 'function_calls', 'x_error', 'interval_or_step'])
        for name, r in results.items():
            writer.writerow([name, r.x, r.value, r.iterations, r.calls, abs(r.x), r.resolution])
    make_plots(results)
    print('Вариант 18. Аналитически: x* = 0, f(x*) = 0.\n' + table)


if __name__ == '__main__':
    main()

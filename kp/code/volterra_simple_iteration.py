"""
Метод простой итерации (метод последовательных приближений Пикара)
для численного решения линейного интегрального уравнения Вольтерра
второго рода:

    y(x) = f(x) + lambda * int_a^x K(x, s) y(s) ds,   x in [a, b]

Курсовой проект. Избосарова Ж.О., гр. Б9123-02.03.01сцт.
"""

import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


def simple_iteration_volterra(f, K, a, b, N, lam=1.0, tol=1e-12, max_iter=10000):
    """
    Решение уравнения Вольтерра II рода методом простой итерации
    с дискретизацией интеграла составной квадратурной формулой трапеций.

    f   : правая часть, f(x) -> np.ndarray
    K   : ядро, K(x, s) -> np.ndarray (векторизовано по s)
    a,b : концы промежутка интегрирования
    N   : число шагов сетки (узлов N+1)
    lam : параметр lambda

    Возвращает: x, y, n_iter, history (список невязок ||y_{n+1}-y_n||_inf)
    """
    x = np.linspace(a, b, N + 1)
    h = (b - a) / N
    F = f(x)

    def quad_integral(Y):
        """Вектор значений int_a^{x_i} K(x_i,s) Y(s) ds по трапециям для всех i."""
        I = np.zeros(N + 1)
        for i in range(1, N + 1):
            w = np.ones(i + 1)
            w[0] = 0.5
            w[-1] = 0.5
            terms = w * K(x[i], x[: i + 1]) * Y[: i + 1]
            I[i] = h * np.sum(terms)
        return I

    Y = F.copy()               # нулевое приближение y_0 = f
    history = []
    n_iter = 0
    for n in range(1, max_iter + 1):
        Y_new = F + lam * quad_integral(Y)
        err = np.max(np.abs(Y_new - Y))
        history.append(err)
        Y = Y_new
        n_iter = n
        if err < tol:
            break
    return x, Y, n_iter, history


def run_example(name, f, K, a, b, lam, exact, Ns, tol=1e-12):
    print(f"\n=== {name} ===")
    print(f"{'N':>6} {'h':>10} {'iter':>6} {'max|err|':>14} {'order p':>10}")
    prev_err = None
    prev_h = None
    for N in Ns:
        x, Y, n_iter, history = simple_iteration_volterra(f, K, a, b, N, lam=lam, tol=tol)
        err = np.max(np.abs(Y - exact(x)))
        h = (b - a) / N
        if prev_err is not None:
            order = np.log(prev_err / err) / np.log(prev_h / h)
            order_str = f"{order:10.3f}"
        else:
            order_str = f"{'--':>10}"
        print(f"{N:6d} {h:10.5f} {n_iter:6d} {err:14.3e} {order_str}")
        prev_err, prev_h = err, h
    return x, Y, exact(x), history


if __name__ == "__main__":
    a, b = 0.0, 1.0
    Ns = [10, 20, 40, 80, 160]

    # Пример 1: y(x) = 1 + \int_0^x (x-s) y(s) ds,  y'' = y, y(0)=1, y'(0)=0
    # Точное решение: y(x) = ch(x)
    f1 = lambda x: np.ones_like(x)
    K1 = lambda x, s: x - s
    exact1 = lambda x: np.cosh(x)
    x1, Y1, E1, hist1 = run_example(
        "Пример 1: y(x)=1+int_0^x(x-s)y(s)ds, точное y=ch(x)",
        f1, K1, a, b, lam=1.0, exact=exact1, Ns=Ns,
    )

    # Пример 2: y(x) = 1 - \int_0^x y(s) ds,  y' = -y, y(0) = 1
    # Точное решение: y(x) = e^{-x}
    f2 = lambda x: np.ones_like(x)
    K2 = lambda x, s: np.ones_like(s)
    exact2 = lambda x: np.exp(-x)
    x2, Y2, E2, hist2 = run_example(
        "Пример 2: y(x)=1-int_0^x y(s)ds, точное y=e^{-x}",
        f2, K2, a, b, lam=-1.0, exact=exact2, Ns=Ns,
    )

    # ---- графики ----
    N_plot = 40
    x1p, Y1p, n1p, hist1p = simple_iteration_volterra(f1, K1, a, b, N_plot, lam=1.0)
    x2p, Y2p, n2p, hist2p = simple_iteration_volterra(f2, K2, a, b, N_plot, lam=-1.0)

    # график 1: приближённое и точное решение
    fig, axes = plt.subplots(1, 2, figsize=(10, 4))
    axes[0].plot(x1p, exact1(x1p), "k-", label="точное $y=\\mathrm{ch}(x)$")
    axes[0].plot(x1p, Y1p, "o", ms=4, mfc="none", label=f"численное, N={N_plot}")
    axes[0].set_title("Пример 1")
    axes[0].set_xlabel("x"); axes[0].set_ylabel("y(x)"); axes[0].legend(); axes[0].grid(alpha=0.3)

    axes[1].plot(x2p, exact2(x2p), "k-", label="точное $y=e^{-x}$")
    axes[1].plot(x2p, Y2p, "o", ms=4, mfc="none", label=f"численное, N={N_plot}")
    axes[1].set_title("Пример 2")
    axes[1].set_xlabel("x"); axes[1].set_ylabel("y(x)"); axes[1].legend(); axes[1].grid(alpha=0.3)
    fig.tight_layout()
    fig.savefig("../images/solutions.png", dpi=150)

    # график 2: сходимость итерационного процесса (невязка по номеру итерации)
    fig2, ax2 = plt.subplots(figsize=(6, 4))
    ax2.semilogy(range(1, len(hist1p) + 1), hist1p, "o-", label="Пример 1")
    ax2.semilogy(range(1, len(hist2p) + 1), hist2p, "s-", label="Пример 2")
    ax2.set_xlabel("номер итерации $n$")
    ax2.set_ylabel(r"$\|y_{n+1}-y_n\|_\infty$")
    ax2.set_title(f"Сходимость метода простой итерации (N={N_plot})")
    ax2.legend(); ax2.grid(alpha=0.3, which="both")
    fig2.tight_layout()
    fig2.savefig("../images/convergence_iterations.png", dpi=150)

    # график 3: ошибка дискретизации в зависимости от шага h (лог-лог)
    hs = [(b - a) / N for N in Ns]
    errs1 = []
    errs2 = []
    for N in Ns:
        xx, YY, _, _ = simple_iteration_volterra(f1, K1, a, b, N, lam=1.0)
        errs1.append(np.max(np.abs(YY - exact1(xx))))
        xx, YY, _, _ = simple_iteration_volterra(f2, K2, a, b, N, lam=-1.0)
        errs2.append(np.max(np.abs(YY - exact2(xx))))

    fig3, ax3 = plt.subplots(figsize=(6, 4))
    ax3.loglog(hs, errs1, "o-", label="Пример 1")
    ax3.loglog(hs, errs2, "s-", label="Пример 2")
    ax3.loglog(hs, [h**2 for h in hs], "k--", label=r"$O(h^2)$")
    ax3.set_xlabel("шаг сетки $h$")
    ax3.set_ylabel(r"$\max_i|y_i - y(x_i)|$")
    ax3.set_title("Порядок сходимости по шагу сетки")
    ax3.legend(); ax3.grid(alpha=0.3, which="both")
    fig3.tight_layout()
    fig3.savefig("../images/error_vs_h.png", dpi=150)

    print("\nГрафики сохранены в kp/images/")

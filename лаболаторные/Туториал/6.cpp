// Лабораторная 6. OpenMP. Перемножение матриц.
// C = A * B для квадратных матриц N x N.
// Распараллеливаем внешний цикл по строкам результата (#pragma omp parallel for).
// Эффективность = t1 / t, где t1 — время на 1 нити, t — на n нитях.
//
// Компиляция (Linux): g++ -o 6.o 6.cpp -fopenmp

#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

// Умножение матриц на заданном числе нитей
void multiply(const vector<vector<double>>& A,
              const vector<vector<double>>& B,
              vector<vector<double>>& C, int N, int threads) {
#pragma omp parallel for num_threads(threads)
    for (int i = 0; i < N; ++i)                 // строка результата — своей нити
        for (int j = 0; j < N; ++j) {
            double s = 0.0;
            for (int k = 0; k < N; ++k)
                s += A[i][k] * B[k][j];
            C[i][j] = s;
        }
}

int main() {
    // ---- Проверка на матрицах 5x5 (единичная * A = A) ----
    {
        int N = 5;
        vector<vector<double>> A(N, vector<double>(N)),
                               I(N, vector<double>(N, 0)),
                               C(N, vector<double>(N));
        for (int i = 0; i < N; ++i) {
            I[i][i] = 1;                         // единичная матрица
            for (int j = 0; j < N; ++j) A[i][j] = i * N + j + 1;
        }
        multiply(A, I, C, N, 2);
        cout << "Проверка 5x5 (A * E должно дать A):\n";
        bool ok = true;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                if (A[i][j] != C[i][j]) ok = false;
        cout << (ok ? "OK\n\n" : "ОШИБКА\n\n");
    }

    // ---- Замеры на большой матрице ----
    const int N = 800;                           // 800x800
    vector<vector<double>> A(N, vector<double>(N, 1.0)),
                           B(N, vector<double>(N, 2.0)),
                           C(N, vector<double>(N));

    // Базовое время на 1 нити
    double t0 = omp_get_wtime();
    multiply(A, B, C, N, 1);
    double t1 = omp_get_wtime() - t0;

    cout << "Потоки\tВремя, с\tЭффективность (t1/t)\n";
    cout << 1 << "\t" << t1 << "\t" << 1.0 << "\n";

    int thread_list[] = {2, 4, 6, 8, 10, 16, 32, 64, 100};
    for (int th : thread_list) {
        double s = omp_get_wtime();
        multiply(A, B, C, N, th);
        double t = omp_get_wtime() - s;
        cout << th << "\t" << t << "\t" << (t1 / t) << "\n";
    }
    return 0;
}

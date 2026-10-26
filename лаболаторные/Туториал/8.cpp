// Лабораторная 8. OpenMP. "Калькулятор" на секциях (sections).
// Две секции = два потока, работающие одновременно:
//   - "клиент" по очереди отсылает строки-команды;
//   - "сервер" принимает строку "A op B", вычисляет и кладёт результат в res.
// Общение идёт ТОЛЬКО через общие переменные + флаги готовности.
// "res" в команде означает предыдущий результат (сначала res = 0).
//
// Компиляция (Linux): g++ -o 8.o 8.cpp -fopenmp

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <omp.h>
using namespace std;

// Общие (shared) переменные для связи клиент <-> сервер
char cmd[64];              // текущая команда
int  res = 0;              // текущий результат (и "res" в выражениях)
bool cmd_ready = false;    // клиент положил новую команду
bool res_ready = false;    // сервер посчитал ответ
bool finished  = false;    // команды закончились

// Превратить операнд в число: "res" -> текущий результат, иначе atoi
int operand(const char* tok) {
    if (strcmp(tok, "res") == 0) return res;
    return atoi(tok);
}

int main() {
    const char* task[] = { "5 + 3", "res / 2", "24 / res",
                           "res * res", "res - 6", nullptr };

#pragma omp parallel sections num_threads(2)
    {
        // ---------------- КЛИЕНТ ----------------
#pragma omp section
        {
            for (int i = 0; task[i] != nullptr; ++i) {
                strcpy(cmd, task[i]);          // кладём команду
                cmd_ready = true;
#pragma omp flush(cmd_ready, cmd)              // делаем запись видимой серверу

                // ждём, пока сервер посчитает
                bool r;
                do {
#pragma omp flush(res_ready)
                    r = res_ready;
                } while (!r);
                res_ready = false;
#pragma omp flush(res_ready)
                cout << "клиент: " << task[i] << "  ->  res = " << res << "\n";
            }
            finished = true;                   // сигнал: команд больше нет
            cmd_ready = true;
#pragma omp flush(finished, cmd_ready)
        }

        // ---------------- СЕРВЕР ----------------
#pragma omp section
        {
            while (true) {
                // ждём новую команду
                bool c;
                do {
#pragma omp flush(cmd_ready)
                    c = cmd_ready;
                } while (!c);
                cmd_ready = false;
#pragma omp flush(cmd_ready, finished)
                if (finished) break;           // клиент закончил

                // разбираем "A op B"
                char a[32], op, b[32];
                sscanf(cmd, "%s %c %s", a, &op, b);
                int x = operand(a), y = operand(b);
                switch (op) {
                    case '+': res = x + y; break;
                    case '-': res = x - y; break;
                    case '*': res = x * y; break;
                    case '/': res = (y != 0) ? x / y : 0; break;
                }
                res_ready = true;
#pragma omp flush(res, res_ready)
            }
        }
    }

    cout << "\nИтоговый результат res = " << res << "\n";
    return 0;
}

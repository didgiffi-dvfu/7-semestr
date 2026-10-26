#include <iostream>
#include <chrono>
#include <thread>
#include "process_runner.h"

using namespace std;

int main()
{
    cout << "=== Кроссплатформенный запуск процессов ===" << endl;

#if defined(_WIN32)
    string test_cmd = "cmd.exe /c timeout /t 3";
#else
    string test_cmd = "sleep 3";
#endif

    cout << "Запуск фоновой команды: \"" << test_cmd << "\"..." << endl;

    ProcessInfo proc = run_in_background(test_cmd);

    if (!proc.isValid)
    {
        cerr << "Не удалось запустить процесс!" << endl;
        return 1;
    }

    cout << "Процесс успешно запущен в фоновом режиме!" << endl;
    cout << "Родительский процесс выполняет параллельную работу..." << endl;

    for (int i = 1; i <= 3; ++i)
    {
        cout << "[Родительский процесс]: Идет работа... (" << i << " сек)" << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }

    cout << "Ожидание завершения дочернего процесса..." << endl;
    int exit_code = -1;

    if (wait_for_process(proc, exit_code))
    {
        cout << ">>> Процесс завершился с кодом возврата (Exit Code): " << exit_code << endl;
    }
    else
    {
        cerr << " Ошибка при ожидании завершения процесса." << endl;
    }

    return 0;
}
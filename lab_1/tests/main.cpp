#include <iostream>
#include <chrono>
#include <thread>
#include "process_runner.h"

int main()
{
    std::cout << "=== Кроссплатформенный запуск процессов ===" << std::endl;

#if defined(_WIN32)
    std::string test_cmd = "cmd.exe /c timeout /t 3";
#else
    std::string test_cmd = "sleep 3";
#endif

    std::cout << "Запуск фоновой команды: \"" << test_cmd << "\"..." << std::endl;

    ProcessInfo proc = run_in_background(test_cmd);

    if (!proc.isValid)
    {
        std::cerr << "Не удалось запустить процесс!" << std::endl;
        return 1;
    }

    std::cout << "Процесс успешно запущен в фоновом режиме!" << std::endl;
    std::cout << "Родительский процесс выполняет параллельную работу..." << std::endl;

    for (int i = 1; i <= 3; ++i)
    {
        std::cout << "[Родительский процесс]: Идет работа... (" << i << " сек)" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "Ожидание завершения дочернего процесса..." << std::endl;
    int exit_code = -1;

    if (wait_for_process(proc, exit_code))
    {
        std::cout << ">>> Процесс завершился с кодом возврата (Exit Code): " << exit_code << std::endl;
    }
    else
    {
        std::cerr << " Ошибка при ожидании завершения процесса." << std::endl;
    }

    return 0;
}
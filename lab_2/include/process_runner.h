#ifndef PROCESS_RUNNER_H
#define PROCESS_RUNNER_H

#include <string>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

typedef HANDLE ProcessHandle;

#else
#include <sys/types.h>
typedef pid_t ProcessHandle;
#endif

struct ProcessInfo
{
    ProcessHandle handle;
    bool isValid;
};

/**
 * Запускает программу в фоновом режиме.
 * @param command_line Команда для выполнения (например, "ping -c 3 127.0.0.1" или "ping 127.0.0.1")
 * @return ProcessInfo с дескриптором созданного процесса
 */
ProcessInfo run_in_background(const std::string &command_line);

/**
 * Ожидает завершения процесса и возвращает его код ответа.
 * @param info Информация о запущенном процессе
 * @param exit_code Указатель на переменную, куда запишется код ответа (exit status)
 * @return true, если процесс завершился успешно и код получен, иначе false
 */
bool wait_for_process(ProcessInfo &info, int &exit_code);

#endif 
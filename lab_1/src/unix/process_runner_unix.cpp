#include "process_runner.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>

ProcessInfo run_in_background(const std::string &command_line)
{
    ProcessInfo info;
    info.isValid = false;
    info.handle = -1;

    pid_t pid = fork();

    if (pid < 0)
    {
        std::cerr << "[POSIX Error] Ошибка вызова fork()" << std::endl;
        return info;
    }

    if (pid == 0)
    {
        // --- Дочерний процесс ---
        // Отвязываем дочерний процесс от текущей сессии для полноценного фонового режима
        setsid();

        // Запуск команды через системную оболочку (sh -c "команда")
        execl("/bin/sh", "sh", "-c", command_line.c_str(), (char *)NULL);

        // Если execl вернул управление — произошла ошибка
        _exit(127);
    }
    else
    {
        // --- Родительский процесс ---
        info.handle = pid;
        info.isValid = true;
    }

    return info;
}

bool wait_for_process(ProcessInfo &info, int &exit_code)
{
    if (!info.isValid || info.handle <= 0)
    {
        return false;
    }

    int status = 0;
    pid_t wpid = waitpid(info.handle, &status, 0);

    if (wpid == info.handle)
    {
        if (WIFEXITED(status))
        {
            exit_code = WEXITSTATUS(status);
            info.isValid = false;
            return true;
        }
    }

    info.isValid = false;
    return false;
}

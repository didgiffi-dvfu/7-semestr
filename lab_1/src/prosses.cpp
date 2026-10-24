#include "process.h"
#include <iostream>
#include <vector>
#include <sstream>

#if defined(_WIN32) || defined(_WIN64)

ProcessInfo run_in_background(const std::string &command_line)
{
    ProcessInfo info;
    info.isValid = false;
    info.handle = INVALID_HANDLE_VALUE;

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Создаем копию строки, так как CreateProcessA может изменять входную строку
    std::vector<char> cmd(command_line.begin(), command_line.end());
    cmd.push_back('\0');

    // Флаг CREATE_NO_WINDOW запускает консольное приложение без создания видимого окна
    BOOL success = CreateProcessA(
        NULL,
        cmd.data(),
        NULL,
        NULL,
        FALSE,
        CREATE_NO_WINDOW,
        NULL,
        NULL,
        &si,
        &pi);

    if (success)
    {
        info.handle = pi.hProcess;
        info.isValid = true;
        // Закрываем дескриптор потока, так как он нам не нужен
        CloseHandle(pi.hThread);
    }
    else
    {
        std::cerr << "[WinAPI Error] Не удалось создать процесс. Код: " << GetLastError() << std::endl;
    }

    return info;
}

bool wait_for_process(ProcessInfo &info, int &exit_code)
{
    if (!info.isValid || info.handle == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    // Ожидаем завершения процесса
    DWORD waitResult = WaitForSingleObject(info.handle, INFINITE);
    if (waitResult == WAIT_OBJECT_0)
    {
        DWORD dwExitCode = 0;
        if (GetExitCodeProcess(info.handle, &dwExitCode))
        {
            exit_code = static_cast<int>(dwExitCode);
            CloseHandle(info.handle);
            info.handle = INVALID_HANDLE_VALUE;
            info.isValid = false;
            return true;
        }
    }

    CloseHandle(info.handle);
    info.handle = INVALID_HANDLE_VALUE;
    info.isValid = false;
    return false;
}

#else // UNIX / POSIX Systems

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
        // Ошибка fork
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

#endif
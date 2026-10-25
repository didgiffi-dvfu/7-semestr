#include "process_runner.h"
#include <iostream>
#include <vector>
#include <sstream>

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

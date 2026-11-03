#include "child_process.h"

#include <vector>
#include <string>


ChildProcess spawn_copy(const std::string &exe_path, const std::string &child_id)
{
    ChildProcess cp;
    cp.valid = false;
    cp.handle = NULL;

    
    std::string cmd = "\"" + exe_path + "\" --child " + child_id;

    std::vector<char> buf(cmd.begin(), cmd.end());
    buf.push_back('\0');

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    BOOL ok = CreateProcessA(
        NULL,          // имя модуля берём из командной строки
        buf.data(),    // командная строка
        NULL, NULL,    // атрибуты безопасности по умолчанию
        FALSE,         // не наследовать дескрипторы
        0,             // без особых флагов
        NULL, NULL,    // окружение и каталог — как у родителя
        &si, &pi);
    if (!ok)
    {
        return cp;
    }

    CloseHandle(pi.hThread);   
    cp.handle = pi.hProcess;  
    cp.valid = true;
    return cp;
}

bool is_running(const ChildProcess &cp)
{
    if (!cp.valid)
    {
        return false;
    }
    DWORD r = WaitForSingleObject(cp.handle, 0);
    return (r == WAIT_TIMEOUT);   
}

void close_child(ChildProcess &cp)
{
    if (!cp.valid)
    {
        return;
    }
    CloseHandle(cp.handle);   
    cp.handle = NULL;
    cp.valid = false;
}

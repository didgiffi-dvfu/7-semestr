#include "child_process.h"

#include <unistd.h>     // fork, execl, _exit
#include <sys/wait.h>   // waitpid, WNOHANG


ChildProcess spawn_copy(const std::string &exe_path, const std::string &child_id)
{
    ChildProcess cp;
    cp.valid = false;
    cp.pid = -1;

    pid_t pid = ::fork();  
    if (pid < 0)
    {
        return cp;          
    }

    if (pid == 0)
    {

        ::execl(exe_path.c_str(), exe_path.c_str(),
                "--child", child_id.c_str(), (char *)nullptr);
        _exit(127);         
    }

    cp.pid = pid;
    cp.valid = true;
    return cp;
}

bool is_running(const ChildProcess &cp)
{
    if (!cp.valid)
    {
        return false;
    }
    int status = 0;
    pid_t r = ::waitpid(cp.pid, &status, WNOHANG);
    // r == 0   -> процесс ещё работает
    // r == pid -> завершился 
    // r <  0   -> ошибка / уже пожат
    return (r == 0);
}

void close_child(ChildProcess &cp)
{
    if (!cp.valid)
    {
        return;
    }

    int status = 0;
    ::waitpid(cp.pid, &status, WNOHANG);
    cp.valid = false;
    cp.pid = -1;
}

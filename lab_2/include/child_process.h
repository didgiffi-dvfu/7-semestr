#ifndef CHILD_PROCESS_H
#define CHILD_PROCESS_H

#include <string>


//    POSIX   -> fork() + execl()      
//    Windows -> CreateProcess()       
//    POSIX   -> waitpid(..., WNOHANG)
//    Windows -> WaitForSingleObject(h, 0)

#if defined(_WIN32) || defined(_WIN64)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <sys/types.h>   // pid_t
#endif

// Дескриптор запущенной копии.
struct ChildProcess
{
    bool valid;
#if defined(_WIN32) || defined(_WIN64)
    HANDLE handle;  
#else
    pid_t  pid;     
#endif
};

ChildProcess spawn_copy(const std::string &exe_path, const std::string &child_id);

bool is_running(const ChildProcess &cp);

void close_child(ChildProcess &cp);

#endif 

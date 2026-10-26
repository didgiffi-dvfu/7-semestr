#include "logger.h"
#include "os_time.h"
#include <string>
// в винде пустой дискриптор файла имеет значение INVALID_HANDLE_VALUE, в юниксе - -1

using namespace std;
Logger::Logger() : handle_(INVALID_HANDLE_VALUE), open_(false) {}

Logger::~Logger()
{
    close();
}

bool Logger::open(const string &path)
{
    if (open_)
    {
        return false; // файл уже открыт
    }

    HANDLE h_file = CreateFileA(path.c_str(),
                                FILE_APPEND_DATA,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);

    if (h_file == INVALID_HANDLE_VALUE)
    {

        return false; // не удалось открыть файл
    }

    handle_ = h_file;
    open_ = true;
    return true;
}

void Logger::log(const string &message)
{
    if (!open_)
    {
        return; // файл не открыт
    }

    TimePoint time_now = date_time_now();
    string formatted_time = date_time_formatter(time_now);

    string log_entry = "|| " + formatted_time + " || PID= " + to_string(current_pid()) + " || " + message + "\n";

    DWORD bytes_written;
    WriteFile(handle_, log_entry.c_str(), log_entry.size(), &bytes_written, NULL);
}

void Logger::close()
{
    if (open_)
    {
        CloseHandle(handle_);
        handle_ = INVALID_HANDLE_VALUE;
        open_ = false;
    }
}

bool Logger::is_open() const
{
    return open_;
}
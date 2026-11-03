#include "logger.h"
#include "os_time.h"

#include <string>
#include <fcntl.h>
#include <unistd.h>

using namespace std;
Logger::Logger() : handle_(-1), open_(false) {}

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

    int fd = ::open(path.c_str(), O_WRONLY  | O_APPEND | O_CREAT, 0644);

    if (fd < 0)
    {

        return false; // не удалось открыть файл
    }

    handle_ = fd;
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
    
    ::write(handle_ , log_entry.data(), log_entry.size());
}

void Logger::close()
{
    if (open_)
    {   
        ::close(handle_);
        handle_ = -1;
        open_ = false;
    }
}

bool Logger::is_open() const
{
    return open_;
}
#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

typedef HANDLE LoggerHandle;

#else
#include <sys/types.h>
typedef int LoggerHandle;
#endif
class Logger
{
public:
    Logger();
    ~Logger(); // дутруктор сам закроет файл

    bool open(const std::string &path);
    void log(const std::string &message);
    void close();
    bool is_open() const;

private:
    LoggerHandle handle_; // дескриптор файла
    bool open_;
};
#endif
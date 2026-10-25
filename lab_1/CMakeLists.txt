# Минимальная поддерживаемая версия CMake
cmake_minimum_required(VERSION 3.10)

# Название проекта
project(ProcessRunner LANGUAGES CXX)

# Стандарт языка C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 1. БИБЛИОТЕКА ---
# Собираем статическую библиотеку из платформенной реализации.
# CMake сам выбирает нужный файл в зависимости от ОС:
#   src/windows/process_runner_win.cpp  — реализация для Windows (WinAPI)
#   src/unix/process_runner_unix.cpp    — реализация для UNIX/macOS (POSIX)

if(WIN32)
    add_library(process_runner STATIC src/windows/process_runner_win.cpp)
else()
    add_library(process_runner STATIC src/unix/process_runner_unix.cpp)
endif()

# Говорим, что заголовки библиотеки лежат в папке include/.
# Благодаря PUBLIC те, кто линкуется с библиотекой (например, тест),
# тоже автоматически увидят эту папку и смогут писать #include "process_runner.h".
target_include_directories(process_runner PUBLIC include)

# --- 2. ТЕСТОВАЯ УТИЛИТА ---
# Собираем исполняемый файл-тест и линкуем его с нашей библиотекой.
add_executable(test_runner tests/main.cpp)
target_link_libraries(test_runner PRIVATE process_runner)

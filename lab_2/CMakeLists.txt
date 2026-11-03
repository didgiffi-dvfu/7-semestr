# Минимальная поддерживаемая версия CMake
cmake_minimum_required(VERSION 3.10)

# Название проекта
project(os_time LANGUAGES CXX)

# Стандарт языка C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Генерировать compile_commands.json — по нему редактор (VS Code / clangd)
# точно видит все include-пути и перестаёт подсвечивать заголовки красным.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- 1. БИБЛИОТЕКА os_time ---
# Модули: время (time_*), логгер (logger_*), общий счётчик (shared_counter_*).
#   Windows: WinAPI
#   UNIX:    POSIX
# Имя библиотеки одинаковое в обеих ветках (os_time).

if(WIN32)
    add_library(os_time STATIC
        src/windows/time_win.cpp
        src/windows/logger_win.cpp
        src/windows/shared_counter_win.cpp)
else()
    add_library(os_time STATIC
        src/unix/time_unix.cpp
        src/unix/logger_unix.cpp
        src/unix/shared_counter_unix.cpp)
endif()

# Заголовки библиотеки лежат в include/. PUBLIC -> их увидит и тест.
target_include_directories(os_time PUBLIC include)

# На Linux функции shm_open()/sem_open() лежат в библиотеках rt и pthread —
# их нужно явно слинковать (на macOS и Windows это не требуется).
if(UNIX AND NOT APPLE)
    target_link_libraries(os_time PUBLIC rt pthread)
endif()

# --- 2. ТЕСТОВАЯ УТИЛИТА ---
# Исполняемый файл-тест, линкуется с библиотекой os_time.
add_executable(test_runner tests/main.cpp)
target_link_libraries(test_runner PRIVATE os_time)

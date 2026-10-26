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
# CMake сам выбирает нужный платформенный файл модуля времени:
#   src/windows/time_win.cpp   — реализация для Windows (WinAPI, GetLocalTime)
#   src/unix/time_unix.cpp     — реализация для UNIX/Linux/macOS (gettimeofday)
# ВАЖНО: имя библиотеки одинаковое в обеих ветках (os_time),
# иначе строки ниже (include/link) ссылались бы на несуществующую цель.

if(WIN32)
    add_library(os_time STATIC src/windows/time_win.cpp)
else()
    add_library(os_time STATIC src/unix/time_unix.cpp)
endif()

# Заголовки библиотеки лежат в include/. PUBLIC -> их увидит и тест.
target_include_directories(os_time PUBLIC include)

# --- 2. ТЕСТОВАЯ УТИЛИТА ---
# Исполняемый файл-тест, линкуется с библиотекой os_time.
add_executable(test_runner tests/main.cpp)
target_link_libraries(test_runner PRIVATE os_time)
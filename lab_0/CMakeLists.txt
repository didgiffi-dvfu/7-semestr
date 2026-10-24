# Минимальная поддерживаемая версия CMake
cmake_minimum_required(VERSION 3.10)

# Название вашего проекта
project(MultiFileProject)

# Указываем стандарт языка C++ (например, C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Указываем, какой исполняемый файл нужно собрать и из каких исходных файлов
add_executable(my_program main.cpp  hello.cpp)
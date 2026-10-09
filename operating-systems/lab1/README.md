# Лабораторная работа: toolchain и Hello, World!

В проекте есть две версии программы:

- `hello_single.c` — однофайловая версия для ручной компиляции через консоль;
- `src/main.c`, `src/greeting.c`, `include/greeting.h` — многофайловая версия, собираемая с помощью Make или CMake.

## 1. Установка toolchain

Нужно выполнить этот раздел в обеих системах и сохранить вывод команд проверки.

### macOS

Установить инструменты разработчика и CMake:

```sh
xcode-select --install
brew install cmake
```

Проверить:

```sh
clang --version
make --version
cmake --version
```

### Ubuntu/Debian Linux

```sh
sudo apt update
sudo apt install build-essential cmake
```

Проверить:

```sh
gcc --version
make --version
cmake --version
```

### Windows с MinGW-w64

1. Скачать архив GCC/MinGW-w64 с `winlibs.com` и распаковать, например, в `C:\\winlibs`.
2. Добавить каталог `C:\\winlibs\\mingw64\\bin` в переменную среды `Path` (точный путь зависит от структуры архива).
3. Установить CMake командой `winget install Kitware.CMake` или с сайта CMake.
4. Открыть новую консоль и проверить:

```powershell
gcc --version
mingw32-make --version
cmake --version
```

## 2. Ручная компиляция однофайловой версии

Все команды выполняются из каталога `operating-systems/lab1`.

macOS (Clang):

```sh
clang -std=c11 -Wall -Wextra -Wpedantic hello_single.c -o hello_single
./hello_single
```

Linux (GCC):

```sh
gcc -std=c11 -Wall -Wextra -Wpedantic hello_single.c -o hello_single
./hello_single
```

Windows (PowerShell + MinGW-w64):

```powershell
gcc -std=c11 -Wall -Wextra -Wpedantic hello_single.c -o hello_single.exe
.\hello_single.exe
```

Ожидаемый результат во всех системах:

```text
Hello, World!
```

## 3. Ручная компиляция многофайловой версии

macOS/Linux:

```sh
cc -std=c11 -Wall -Wextra -Wpedantic -Iinclude src/main.c src/greeting.c -o hello
./hello
```

Windows:

```powershell
gcc -std=c11 -Wall -Wextra -Wpedantic -Iinclude src/main.c src/greeting.c -o hello.exe
.\hello.exe
```

## 4. Сборка с Make

macOS/Linux:

```sh
make
make run
make test
```

В Windows используйте `mingw32-make`:

```powershell
mingw32-make
mingw32-make run
mingw32-make test
```

## 5. Сборка с CMake

macOS/Linux:

```sh
cmake -S . -B build
cmake --build build
./build/hello
ctest --test-dir build --output-on-failure
```

Windows с MinGW-w64:

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
.\build\hello.exe
ctest --test-dir build --output-on-failure
```

Если каталог `build` уже создавался другим генератором, удалите его перед повторной конфигурацией.

## 6. Что приложить к отчёту

Для каждой из двух систем приложите:

1. вывод версий компилятора, Make и CMake;
2. команду ручной компиляции `hello_single.c` и результат запуска;
3. команды конфигурации и сборки CMake;
4. результат запуска многофайловой программы или успешный вывод `ctest`.

@echo off
REM =============================================================
REM  Скрипт автоматизации для Windows
REM  Что делает:
REM    1. Обновляет исходники из git-репозитория (git pull)
REM    2. Конфигурирует проект через CMake (режим Debug -> есть
REM       отладочная информация для отладчика)
REM    3. Компилирует проект
REM
REM  Использование:
REM    build.cmd            - собрать lab_1 (по умолчанию)
REM    build.cmd lab_0      - собрать другую лабу
REM =============================================================

REM Папка проекта: первый аргумент или lab_1 по умолчанию
set "PROJECT_DIR=%~1"
if "%PROJECT_DIR%"=="" set "PROJECT_DIR=lab_1"

REM Переходим в директорию, где лежит сам скрипт (корень репозитория)
cd /d "%~dp0"

echo === [1/3] Обновление исходников из Git ===
git pull
if errorlevel 1 goto :error

echo.
echo === [2/3] Конфигурация CMake (%PROJECT_DIR%) ===
cmake -S "%PROJECT_DIR%" -B "%PROJECT_DIR%\build" -DCMAKE_BUILD_TYPE=Debug
if errorlevel 1 goto :error

echo.
echo === [3/3] Компиляция ===
cmake --build "%PROJECT_DIR%\build" --config Debug
if errorlevel 1 goto :error

echo.
echo === Готово! Исполняемые файлы в %PROJECT_DIR%\build\ ===
goto :eof

:error
echo.
echo !!! Ошибка при выполнении сборки !!!
exit /b 1

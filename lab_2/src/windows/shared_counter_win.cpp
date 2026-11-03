#include "shared_counter.h"

//  Windows-реализация разделяемой памяти (Лекция 3, слайд 25):
//    CreateFileMapping — создать/открыть именованную область памяти
//    MapViewOfFile     — подключить её -> получить указатель
//    UnmapViewOfFile   — отключить
//    CreateMutex       — именованный мьютекс-лок (в Windows именованным
//                        может быть и мьютекс)


SharedCounter::SharedCounter()
    : data_(nullptr), open_(false),
      map_handle_(NULL), mutex_handle_(NULL) {}

SharedCounter::~SharedCounter()
{
    close();
}

bool SharedCounter::open(const std::string &name)
{
    if (open_)
    {
        return false;
    }

    // Префикс "Local\\" — объект виден в пределах сессии пользователя.
    const std::string map_name = "Local\\" + name;
    const std::string mtx_name = "Local\\" + name + "_lock";

    // 1) Создаём/открываем именованную область памяти заданного размера.
    //    INVALID_HANDLE_VALUE в первом аргументе значит "память, а не файл на диске".
    map_handle_ = CreateFileMappingA(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0, sizeof(SharedData),
        map_name.c_str());
    if (map_handle_ == NULL)
    {
        return false;
    }

    // Если объект уже существовал — его создал другой процесс.
    // Значит, инициализировать счётчик нам НЕ нужно.
    bool created = (GetLastError() != ERROR_ALREADY_EXISTS);

    // 2) Подключаем область в адресное пространство -> получаем указатель.
    void *addr = MapViewOfFile(map_handle_, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedData));
    if (addr == NULL)
    {
        return false;
    }
    data_ = static_cast<SharedData *>(addr);

    // 3) Именованный мьютекс-лок (не захвачен при создании: bInitialOwner = FALSE).
    mutex_handle_ = CreateMutexA(NULL, FALSE, mtx_name.c_str());
    if (mutex_handle_ == NULL)
    {
        return false;
    }

    // 4) Инициализируем счётчик нулём только если область создана нами.
    if (created)
    {
        lock();
        data_->counter = 0;
        unlock();
    }

    open_ = true;
    return true;
}

void SharedCounter::lock()
{
    WaitForSingleObject(mutex_handle_, INFINITE);  // захватить мьютекс
}

void SharedCounter::unlock()
{
    ReleaseMutex(mutex_handle_);  // отпустить мьютекс
}

void SharedCounter::close()
{
    if (!open_)
    {
        return;
    }
    if (data_)
    {
        UnmapViewOfFile(data_);
        data_ = nullptr;
    }
    if (map_handle_)
    {
        CloseHandle(map_handle_);
        map_handle_ = NULL;
    }
    if (mutex_handle_)
    {
        CloseHandle(mutex_handle_);
        mutex_handle_ = NULL;
    }
    open_ = false;
    // В Windows объект памяти сам исчезает, когда закрыт последний HANDLE.
}

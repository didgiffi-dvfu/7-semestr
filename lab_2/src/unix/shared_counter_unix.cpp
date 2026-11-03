#include "shared_counter.h"

#include <sys/mman.h>   // shm_open, mmap, munmap
#include <sys/stat.h>   // права доступа (mode)
#include <fcntl.h>      // флаги O_*
#include <unistd.h>     // ftruncate, close


//    shm_open   — создать/открыть именованный сегмент памяти
//    ftruncate  — задать его размер
//    mmap       — подключить сегмент в адресное пространство -> указатель
//    sem_open   — именованный семафор-лок (в POSIX межпроцессным именованным
//                 может быть ТОЛЬКО семафор)

SharedCounter::SharedCounter()
    : data_(nullptr), open_(false), shm_fd_(-1), sem_(SEM_FAILED) {}

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

    // Имена объектов в POSIX начинаются со слэша и состоят из одного компонента.
    const std::string shm_name = "/" + name;          // имя сегмента памяти
    const std::string sem_name = "/" + name + "_lock"; // имя семафора-лока


    
    bool created = true;
    int fd = ::shm_open(shm_name.c_str(), O_CREAT | O_EXCL | O_RDWR, 0666);
    if (fd < 0)
    {
        // Сегмент уже существует (его создал другой процесс) -> просто открываем.
        created = false;
        fd = ::shm_open(shm_name.c_str(), O_RDWR, 0666);
        if (fd < 0)
        {
            return false;
        }
    }
    shm_fd_ = fd;

    // 2) Если создали впервые — задаём размер сегмента под нашу структуру.
    if (created)
    {
        if (::ftruncate(fd, sizeof(SharedData)) != 0)
        {
            return false;
        }
    }

    // 3) Подключаем сегмент в своё адресное пространство. MAP_SHARED означает,
    //    что изменения видны всем процессам, подключённым к этому сегменту.
    void *addr = ::mmap(nullptr, sizeof(SharedData),
                        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
    {
        return false;
    }
    data_ = static_cast<SharedData *>(addr);

    // 4) Открываем/создаём именованный семафор-лок с начальным значением 1
    //    (1 = «свободен»; sem_wait уменьшит до 0 = «занят»).
    sem_ = ::sem_open(sem_name.c_str(), O_CREAT, 0666, 1);
    if (sem_ == SEM_FAILED)
    {
        return false;
    }

    // 5) Инициализируем счётчик нулём — только если сегмент создан нами.
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
    ::sem_wait(sem_);  // захватить: ждём, пока семафор станет > 0, и уменьшаем
}

void SharedCounter::unlock()
{
    ::sem_post(sem_);  // отпустить: увеличиваем семафор
}

void SharedCounter::close()
{
    if (!open_)
    {
        return;
    }
    if (data_)
    {
        ::munmap(data_, sizeof(SharedData));  // отключаем память
        data_ = nullptr;
    }
    if (shm_fd_ >= 0)
    {
        ::close(shm_fd_);
        shm_fd_ = -1;
    }
    if (sem_ != SEM_FAILED)
    {
        ::sem_close(sem_);
        sem_ = SEM_FAILED;
    }
    open_ = false;


}

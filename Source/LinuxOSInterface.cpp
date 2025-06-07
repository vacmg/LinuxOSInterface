#include "LinuxOSInterface.h"
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <mutex>
#include <semaphore>
#include <thread>

#define CONFIG_USE_BUSY_SLEEP 0

timespec msToTimespec(uint32_t ms)
{
    timespec ts{};
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += ms / 1000;
    ts.tv_nsec += (ms % 1000) * 1000000;

    // Handle overflow of nanoseconds
    if (ts.tv_nsec >= 1000000000)
    {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000;
    }
    return ts;
}

class linuxMutex final : public OSInterface_Mutex
{
public:
    linuxMutex()
    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
        pthread_mutex_init(&mutex, &attr);
    }

    ~linuxMutex() override
    {
        pthread_mutex_destroy(&mutex);
    }

    void signal() override
    {
        if (const error_t res = pthread_mutex_unlock(&mutex); res != 0)
        {
            OSInterfaceLogError("LinuxOSInterface", "Failed to unlock mutex: %s", strerror(res));
            exit(EXIT_FAILURE);
        }
    }

    bool wait(uint32_t max_time_to_wait_ms) override
    {
        const timespec ts  = msToTimespec(max_time_to_wait_ms);
        error_t        res = pthread_mutex_timedlock(&mutex, &ts);
        if (res != 0)
        {
            OSInterfaceLogError("LinuxOSInterface", "Failed to lock mutex: %s", strerror(res));
        }
        return res == 0;
    }

private:
    pthread_mutex_t mutex{};
};

class linuxBinarySemaphore final : public OSInterface_BinarySemaphore
{
public:
    linuxBinarySemaphore()
    {
        if (sem_init(&semaphore, 0, 0) == -1)
        {
            OSInterfaceLogError("LinuxOSInterface", "Failed to initialize semaphore: %s", strerror(errno));
            exit(errno);
        }
    }

    ~linuxBinarySemaphore() override
    {
        if (sem_destroy(&semaphore) == -1)
        {
            OSInterfaceLogError("LinuxOSInterface", "Failed to destroy semaphore: %s", strerror(errno));
            exit(errno);
        }
    }

    void signal() override
    {
        if (sem_post(&semaphore) == -1)
        {
            OSInterfaceLogError("LinuxOSInterface", "Failed to signal semaphore: %s", strerror(errno));
            exit(errno);
        }
    }

    bool wait(uint32_t max_time_to_wait_ms) override
    {
        const timespec ts  = msToTimespec(max_time_to_wait_ms);
        error_t        res = sem_timedwait(&semaphore, &ts);
        if (res == -1)
        {
            OSInterfaceLogError("LinuxOSInterface", "Failed to wait on semaphore: %s", strerror(errno));
        }
        return res == 0;
    }

private:
    sem_t semaphore{};
};

uint32_t LinuxOSInterface::osMillis()
{
    timespec ts{};
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

#if CONFIG_USE_BUSY_SLEEP
void linuxSleep(uint32_t ms)
{
    uint32_t start = linuxMillis();
    while (linuxMillis() - start < ms)
        ;
}
#else
void LinuxOSInterface::osSleep(const uint32_t ms)
{
    timespec ts{};
    ts.tv_sec  = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, nullptr);
}
#endif

OSInterface_Mutex* LinuxOSInterface::osCreateMutex()
{
    return new linuxMutex();
}

OSInterface_BinarySemaphore* LinuxOSInterface::osCreateBinarySemaphore()
{
    return new linuxBinarySemaphore();
}

void* LinuxOSInterface::osMalloc(const uint32_t size)
{
    return size == 0 ? nullptr : malloc(size);
}

void LinuxOSInterface::osFree(void* ptr)
{
    free(ptr);
}

void LinuxOSInterface::osRunProcess(OSInterfaceProcess process, void* arg)
{
    osRunProcess(process, "NewProcess", arg);
}

void LinuxOSInterface::osRunProcess(OSInterfaceProcess process, const char* processName, void* arg)
{
    OSInterfaceLogInfo("OSInterface", "Running process %s", processName);
    std::thread t(process, arg);
    t.detach();
}

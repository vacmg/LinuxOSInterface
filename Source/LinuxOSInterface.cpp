#include "LinuxOSInterface.h"
#include <cstdlib>
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
        if (pthread_mutex_unlock(&mutex) != 0)
        {
            exit(EXIT_FAILURE);
        }
    }

    bool wait(uint32_t max_time_to_wait_ms) override
    {
        const timespec ts = msToTimespec(max_time_to_wait_ms);
        return pthread_mutex_timedlock(&mutex, &ts) == 0;
    }

private:
    pthread_mutex_t mutex{};
};

class linuxBinarySemaphore final : public OSInterface_BinarySemaphore
{
public:
    linuxBinarySemaphore()
    {
        sem_init(&semaphore, 0, 0);
    }

    ~linuxBinarySemaphore() override
    {
        sem_destroy(&semaphore);
    }

    void signal() override
    {
        sem_post(&semaphore);
    }

    bool wait(uint32_t max_time_to_wait_ms) override
    {
        const timespec ts = msToTimespec(max_time_to_wait_ms);
        return sem_timedwait(&semaphore, &ts) == 0;
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

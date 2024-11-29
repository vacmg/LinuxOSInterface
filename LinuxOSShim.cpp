#include "LinuxOSShim.h"
#include <cstdlib>
#include <ctime>
#include <mutex>

#define CONFIG_USE_BUSY_SLEEP 0

class linuxMutex final : public OSShim_Mutex
{
public:
    linuxMutex() { pthread_mutex_init(&mutex, nullptr); }
    ~linuxMutex() override { pthread_mutex_destroy(&mutex); }
    void signal() override { pthread_mutex_unlock(&mutex); }
    bool wait(uint32_t max_time_to_wait_ms) override
    {
        timespec ts{};
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += max_time_to_wait_ms / 1000;
        ts.tv_nsec += (max_time_to_wait_ms % 1000) * 1000000;

        // Handle overflow of nanoseconds
        if (ts.tv_nsec >= 1000000000)
        {
            ts.tv_sec += 1;
            ts.tv_nsec -= 1000000000;
        }

        return pthread_mutex_timedlock(&mutex, &ts) == 0;
    }

private:
    pthread_mutex_t mutex{};
};

uint32_t LinuxOSShim::osMillis()
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
void LinuxOSShim::osSleep(uint32_t ms)
{
    timespec ts{};
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, nullptr);
}
#endif

OSShim_Mutex* LinuxOSShim::osCreateMutex() { return new linuxMutex(); }

void* LinuxOSShim::osMalloc(uint32_t size) { return malloc(size); }

void LinuxOSShim::osFree(void* ptr) { free(ptr); }

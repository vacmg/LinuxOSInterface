#include "LinuxOSInterface.h"
#include "gtest/gtest.h"

static LinuxOSInterface linuxOSInterface;

TEST(LinuxOSInterface, timeTest)
{
    uint32_t timeToSleep = 10;
    uint32_t repeat      = 10;
    bool     flag        = false;

    for (uint32_t i = 0; i < repeat; i++)
    {
        uint32_t millis = linuxOSInterface.osMillis();
        linuxOSInterface.osSleep(timeToSleep);
        uint32_t millis2 = linuxOSInterface.osMillis();

        EXPECT_GE(millis2, millis + timeToSleep);
        if (millis2 >= millis + timeToSleep + 100)
        {
            flag = true;
        }
    }

    if (flag)
    {
        FAIL() << "Sleep slept for too long";
    }
}

TEST(LinuxOSInterface, osMallocSimpleAlloc)
{
    void* ptr = linuxOSInterface.osMalloc(100);
    ASSERT_NE(ptr, nullptr);
    linuxOSInterface.osFree(ptr);
}

TEST(LinuxOSInterface, osMallocZeroAlloc)
{
    void* ptr = linuxOSInterface.osMalloc(0);
    ASSERT_EQ(ptr, nullptr);
    linuxOSInterface.osFree(ptr);
}

TEST(LinuxOSInterface, osMallocLargeAlloc)
{
    void* ptr = linuxOSInterface.osMalloc(1024 * 1024 * 6);
    ASSERT_NE(ptr, nullptr);
    linuxOSInterface.osFree(ptr);
}

TEST(LinuxOSInterface, osMallocMultipleAlloc)
{
    void* ptr1 = linuxOSInterface.osMalloc(100);
    void* ptr2 = linuxOSInterface.osMalloc(100);
    void* ptr3 = linuxOSInterface.osMalloc(100);
    ASSERT_NE(ptr1, nullptr);
    ASSERT_NE(ptr2, nullptr);
    ASSERT_NE(ptr3, nullptr);
    linuxOSInterface.osFree(ptr1);
    linuxOSInterface.osFree(ptr2);
    linuxOSInterface.osFree(ptr3);
}

TEST(LinuxOSInterface, mutexWait)
{
    OSInterface_Mutex* mutex = linuxOSInterface.osCreateMutex();
    EXPECT_TRUE(mutex != nullptr);
    EXPECT_TRUE(mutex->wait(10));
    delete mutex;
}

TEST(LinuxOSInterface, mutexTestNormal)
{
    OSInterface_Mutex* mutex = linuxOSInterface.osCreateMutex();
    ASSERT_NE(mutex, nullptr);

    // Lock the mutex
    ASSERT_TRUE(mutex->wait(10000));
    // Flag to check if the second thread was able to lock the mutex
    volatile bool secondThreadLocked = false;

    // Create a second thread that tries to lock the mutex
    std::thread t(
        [&]
        {
            EXPECT_TRUE(mutex->wait(10000));
            secondThreadLocked = true;
            mutex->signal();
        });

    // Sleep for a short time to ensure the second thread attempts to lock the mutex
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // The second thread should not have been able to lock the mutex yet
    EXPECT_FALSE(secondThreadLocked);

    // Unlock the mutex
    mutex->signal();

    // Wait for the second thread to finish
    t.join();

    // Now the second thread should have been able to lock the mutex
    EXPECT_TRUE(secondThreadLocked);
    delete mutex;
}

TEST(LinuxOSInterface, mutexTestTimeout)
{
    OSInterface_Mutex* mutex = linuxOSInterface.osCreateMutex();
    ASSERT_NE(mutex, nullptr);

    // Lock the mutex
    ASSERT_TRUE(mutex->wait(10000));
    // Flag to check if the second thread was able to lock the mutex
    volatile bool secondThreadLocked = false;

    // Create a second thread that tries to lock the mutex
    std::thread t(
        [&]
        {
            auto res = mutex->wait(50);
            EXPECT_FALSE(res);
            if (res)
            {
                secondThreadLocked = true;
                mutex->signal();
            }
        });

    // Sleep for a short time to ensure the second thread attempts to lock the mutex
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // The second thread should not have been able to lock the mutex yet
    EXPECT_FALSE(secondThreadLocked);

    // Unlock the mutex
    mutex->signal();

    // Wait for the second thread to finish
    t.join();

    EXPECT_FALSE(secondThreadLocked);
    delete mutex;
}

TEST(LinuxOSInterface, binarySemaphoreInit)
{
    OSInterface_BinarySemaphore* semaphore = linuxOSInterface.osCreateBinarySemaphore();
    EXPECT_TRUE(semaphore != nullptr);
    EXPECT_FALSE(semaphore->wait(10));
    delete semaphore;
}

TEST(LinuxOSInterface, binarySemaphoreWaitSignal)
{
    OSInterface_BinarySemaphore* semaphore = linuxOSInterface.osCreateBinarySemaphore();
    EXPECT_TRUE(semaphore != nullptr);
    semaphore->signal();
    EXPECT_TRUE(semaphore->wait(10));
    delete semaphore;
}

TEST(LinuxOSInterface, binarySemaphoreWaitSignalWait)
{
    OSInterface_BinarySemaphore* semaphore = linuxOSInterface.osCreateBinarySemaphore();
    EXPECT_TRUE(semaphore != nullptr);
    semaphore->signal();
    EXPECT_TRUE(semaphore->wait(10));
    EXPECT_FALSE(semaphore->wait(10));
    delete semaphore;
}

TEST(LinuxOSInterface, semaphoreTestNormal)
{
    OSInterface_BinarySemaphore* semaphore = linuxOSInterface.osCreateBinarySemaphore();
    ASSERT_NE(semaphore, nullptr);
    semaphore->signal();

    // Lock the semaphore
    ASSERT_TRUE(semaphore->wait(10000));
    // Flag to check if the second thread was able to lock the semaphore.
    volatile bool secondThreadLocked = false;

    // Create a second thread that tries to lock the semaphore
    std::thread t(
        [&]
        {
            EXPECT_TRUE(semaphore->wait(10000));
            secondThreadLocked = true;
            semaphore->signal();
        });

    // Sleep for a short time to ensure the second thread attempts to lock the semaphore
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // The second thread should not have been able to lock the semaphore yet
    EXPECT_FALSE(secondThreadLocked);

    // Unlock the semaphore
    semaphore->signal();

    // Wait for the second thread to finish
    t.join();

    // Now the second thread should have been able to lock the semaphore
    EXPECT_TRUE(secondThreadLocked);
    delete semaphore;
}

TEST(LinuxOSInterface, semaphoreTestTimeout)
{
    OSInterface_Mutex* semaphore = linuxOSInterface.osCreateMutex();
    ASSERT_NE(semaphore, nullptr);
    semaphore->signal();

    // Lock the semaphore
    ASSERT_TRUE(semaphore->wait(10000));
    // Flag to check if the second thread was able to lock the semaphore
    volatile bool secondThreadLocked = false;

    // Create a second thread that tries to lock the semaphore
    std::thread t(
        [&]
        {
            auto res = semaphore->wait(50);
            EXPECT_FALSE(res);
            if (res)
            {
                secondThreadLocked = true;
                semaphore->signal();
            }
        });

    // Sleep for a short time to ensure the second thread attempts to lock the semaphore
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // The second thread should not have been able to lock the semaphore yet
    EXPECT_FALSE(secondThreadLocked);

    // Unlock the semaphore
    semaphore->signal();

    // Wait for the second thread to finish
    t.join();

    EXPECT_FALSE(secondThreadLocked);
    delete semaphore;
}

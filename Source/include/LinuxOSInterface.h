#ifndef DOCANCPPLIBTEST_OSLINUXSHIM_H
#define DOCANCPPLIBTEST_OSLINUXSHIM_H

#include "OSInterface.h"

class LinuxOSInterface : public OSInterface
{
public:
    void osSleep(uint32_t ms) override;
    uint32_t osMillis() override;
    OSInterface_Mutex* osCreateMutex() override;
    OSInterface_BinarySemaphore* osCreateBinarySemaphore() override;

    void* osMalloc(uint32_t size) override;
    void osFree(void* ptr) override;
};

#endif // DOCANCPPLIBTEST_OSLINUXSHIM_H

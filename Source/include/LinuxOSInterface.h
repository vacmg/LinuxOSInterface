#ifndef OSLINUXINTERFACE_H
#define OSLINUXINTERFACE_H

#include "OSInterface.h"

class LinuxOSInterface : public OSInterface
{
public:
    void                         osSleep(uint32_t ms) override;
    uint32_t                     osMillis() override;
    OSInterface_Mutex*           osCreateMutex() override;
    OSInterface_BinarySemaphore* osCreateBinarySemaphore() override;

    void* osMalloc(uint32_t size) override;
    void  osFree(void* ptr) override;
    void  osRunProcess(OSInterfaceProcess process, void* arg) override;
    void  osRunProcess(OSInterfaceProcess process, const char* processName, void* arg) override;
};

#endif // OSLINUXINTERFACE_H

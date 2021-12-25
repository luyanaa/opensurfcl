#pragma once
#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
    #include <OpenCL/OpenCL.h>
#else
    #include <CL/cl.h>
#endif

class CCLDevice
{
public:
    friend class CSurfCL;
    //Die Werte entsprechen den Gerätetypen von OpenCL
    enum EDeviceType
    {
        CPU = (1 << 1),//CL_DEVICE_TYPE_CPU,
        GPU = (1 << 2),//CL_DEVICE_TYPE_GPU,
        ACCELERATOR = (1 << 3)//CL_DEVICE_TYPE_ACCELERATOR
    };
private:
    cl_device_id m_DeviceId;

public:
    CCLDevice();
    CCLDevice(cl_device_id _Id);
    CCLDevice(const CCLDevice& _rCpy);
    ~CCLDevice(void);

    unsigned int GetNameLen();
    void GetName(char* name);
    EDeviceType GetType();

};


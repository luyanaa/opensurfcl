#include "CCLDevice.h"
#include <assert.h>

CCLDevice::CCLDevice()
{
    m_DeviceId = NULL;
}
CCLDevice::CCLDevice(cl_device_id _Id)
{
    m_DeviceId = _Id;
}

CCLDevice::CCLDevice(const CCLDevice& _rCpy)
{
    m_DeviceId = _rCpy.m_DeviceId;
}


CCLDevice::~CCLDevice(void)
{

}

unsigned int CCLDevice::GetNameLen()
{
    size_t len = 0;
    cl_int ErrorCode = clGetDeviceInfo(m_DeviceId, CL_DEVICE_NAME, 0, 0, &len);
	assert(ErrorCode == CL_SUCCESS);

    return len;
}
void CCLDevice::GetName(char* name)
{
    cl_int ErrorCode = clGetDeviceInfo(m_DeviceId, CL_DEVICE_NAME, GetNameLen(), name, NULL);
	assert(ErrorCode == CL_SUCCESS);
}

CCLDevice::EDeviceType CCLDevice::GetType()
{
    cl_device_type result = 0;
    cl_int ErrorCode = clGetDeviceInfo(m_DeviceId, CL_DEVICE_TYPE, sizeof(cl_device_type), &result, NULL);
	assert(ErrorCode == CL_SUCCESS);
    return static_cast<CCLDevice::EDeviceType>(result);
}
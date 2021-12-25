/// This code is based on the orginal OpenSurf Implementation by Chris Evans!
/// http://code.google.com/p/opensurf1

#include "CSurfCL.h"
#include <iostream>
#include <fstream>
#include <assert.h>
#include "math.h"

CSurfCL::CSurfCL()
{
    cl_int ErrorCode;

    cl_device_id DeviceID;
    //get the first device
    ErrorCode = CSurfCL::GetFirstDevice(DeviceID);
    assert(ErrorCode == CL_SUCCESS);

    Init(DeviceID);
}

CSurfCL::CSurfCL(CCLDevice& device)
{
    Init(device.m_DeviceId);
}

CSurfCL::~CSurfCL(void)
{
    cl_int ErrorCode = CL_SUCCESS;
    if(m_IntegralImageInput != NULL) 
    {
        ErrorCode = clReleaseMemObject(m_IntegralImageInput);
        assert(ErrorCode == CL_SUCCESS);    
    }

    if(m_IntegralImageOutput != NULL) 
    {
        ErrorCode = clReleaseMemObject(m_IntegralImageOutput);
        assert(ErrorCode == CL_SUCCESS);
    }

    if(m_IntegralImageBuffer != NULL)
    {
        ErrorCode = clReleaseMemObject(m_IntegralImageBuffer);
        assert(ErrorCode == CL_SUCCESS);
    }

    //static Buffer
    ErrorCode = clReleaseMemObject(m_HessianResponseBuffer);
    assert(ErrorCode == CL_SUCCESS);    

    if(m_HessianIpointsBuffer != NULL) 
    {
        ErrorCode = clReleaseMemObject(m_HessianIpointsBuffer);
        assert(ErrorCode == CL_SUCCESS);
    }

    if(m_HessianRBuffer != NULL) 
    {
        ErrorCode = clReleaseMemObject(m_HessianRBuffer);
        assert(ErrorCode == CL_SUCCESS);
    }

    if(m_HessianLBuffer != NULL) 
    {
        ErrorCode = clReleaseMemObject(m_HessianLBuffer);
        assert(ErrorCode == CL_SUCCESS);
    }

    if(m_IPointsCountBuff != NULL) 
    {
        ErrorCode = clReleaseMemObject(m_IPointsCountBuff);
        assert(ErrorCode == CL_SUCCESS);
    }

    if(m_DIpointBuffer != NULL) 
    {
        ErrorCode = clReleaseMemObject(m_DIpointBuffer);
        assert(ErrorCode == CL_SUCCESS);
    }

    if (m_Ipoints != NULL)
    {
        delete[] m_Ipoints;
    }

    if (m_responselayers != NULL)
    {
        delete[] m_responselayers;
    }

    if (m_IPointsCount != NULL) delete[] m_IPointsCount;

    clReleaseKernel(m_IntegralImageKernel);
    clReleaseProgram(m_IntegralImageProgram);

    clReleaseKernel(m_HessianKernel);
    clReleaseProgram(m_HessianProgram);

    clReleaseKernel(m_ResponseKernel);
    clReleaseProgram(m_ResponseProgram);

    clReleaseKernel(m_IPointsKernel);
    clReleaseProgram(m_IPointsProgram);

    clReleaseKernel(m_SurfDescriptorKernel);
    clReleaseProgram(m_SurfDescriptorProgram);


    clReleaseContext(m_Context);
}

void CSurfCL::Init(cl_device_id& device)
{
    cl_int ErrorCode;
    m_DeviceID = device;

    ErrorCode = clGetDeviceInfo(device, CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &m_UseImages, 0);
    assert(ErrorCode == CL_SUCCESS);
	
#ifdef __APPLE__
	m_UseImages = 0;
#endif
	
    //create context
    m_Context = clCreateContext(0, 1, &m_DeviceID, ErrorHandler, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);


    //create command queue
    m_CommandQueue = clCreateCommandQueue(m_Context, m_DeviceID, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);


    //create Test programm part
    CreateCLProgram(DATA_DIRECTORY"/test.cl", "TestCL", m_TestProgram, m_TestKernel);
    //testbuffer
    m_TestBuffer = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, sizeof(cl_float), NULL, &ErrorCode);    
    assert(ErrorCode == CL_SUCCESS);



    //IntegralImage
    if(m_UseImages == CL_TRUE)
    {
        CreateCLProgram(DATA_DIRECTORY"/IntegralImage.cl", "CreateIntegralImageParallelV3", m_IntegralImageProgram, m_IntegralImageKernel);	
    }
    else
    {
        CreateCLProgram(DATA_DIRECTORY"/IntegralImage.cl", "CreateIntegralImageParallelV2", m_IntegralImageProgram, m_IntegralImageKernel);	
    }

    clGetKernelWorkGroupInfo(m_IntegralImageKernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &m_IntegralImageKernelWorkSize, NULL);

    // create Hesian program
    CreateCLProgram(DATA_DIRECTORY"/Hessian.cl", "FastHessianParallel", m_HessianProgram, m_HessianKernel);	
    clGetKernelWorkGroupInfo(m_HessianKernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &m_HessianKernelWorkSize, NULL);

    if(m_UseImages == CL_TRUE)
    {
        // create Response program
        CreateCLProgram(DATA_DIRECTORY"/Hessian.cl", "ResponseLayerParallelV4", m_ResponseProgram, m_ResponseKernel);	
        clGetKernelWorkGroupInfo(m_ResponseKernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &m_ResponseKernelWorkSize, NULL);
    }
    else
    {
        // create Response program
        CreateCLProgram(DATA_DIRECTORY"/Hessian.cl", "ResponseLayerParallelV2", m_ResponseProgram, m_ResponseKernel);	
        clGetKernelWorkGroupInfo(m_ResponseKernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &m_ResponseKernelWorkSize, NULL);
    }

    if(m_UseImages == CL_TRUE)
    {
        // create IPoints program
        CreateCLProgram(DATA_DIRECTORY"/Hessian.cl", "IPointsParallelV2Image", m_IPointsProgram, m_IPointsKernel);
        clGetKernelWorkGroupInfo(m_IPointsKernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &m_IPointsKernelWorkSize, NULL);
    }
    else
    {
        CreateCLProgram(DATA_DIRECTORY"/Hessian.cl", "IPointsParallelV2", m_IPointsProgram, m_IPointsKernel);
        clGetKernelWorkGroupInfo(m_IPointsKernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &m_IPointsKernelWorkSize, NULL);
    }

    // create GetIPoints program
    CreateCLProgram(DATA_DIRECTORY"/Hessian.cl", "GetIPoints", m_GetIPointsProgram, m_GetIPointsKernel);

    // create SurfDescriptor program
    if(m_UseImages == CL_TRUE)
    {
        CreateCLProgram(DATA_DIRECTORY"/SurfDescriptor.cl", "SurfDescriptorParallelImage", m_SurfDescriptorProgram, m_SurfDescriptorKernel);	
        clGetKernelWorkGroupInfo(m_SurfDescriptorKernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &m_SurfDescriptorKernelWorkSize, NULL);
    }
    else
    {
        CreateCLProgram(DATA_DIRECTORY"/SurfDescriptor.cl", "SurfDescriptorParallel", m_SurfDescriptorProgram, m_SurfDescriptorKernel);	
        clGetKernelWorkGroupInfo(m_SurfDescriptorKernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &m_SurfDescriptorKernelWorkSize, NULL);
    }

    //static buffers
    m_HessianResponseBuffer = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, 
        sizeof(SResponseLayer) * countresponselayers, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

    //set Image Properties to 0.
    m_Width = 0;
    m_Height = 0;

    //set Buffers to NULL
    m_IntegralImageInput = NULL;
    m_IntegralImageOutput = NULL;
    m_IntegralImageBuffer = NULL;

    m_HessianIpointsBuffer = NULL;
    m_HessianLBuffer = NULL;
    m_HessianRBuffer = NULL;

    m_Ipoints = NULL;
    m_IPointsCountBuff = NULL;
    m_responselayers = NULL;

    m_DIpointBuffer = NULL;
    m_GetIpoints = NULL;
    m_TmpIpoints = NULL;
    m_IPointsCount = NULL;

    if (m_Ipoints != NULL)
    {
        delete[] m_Ipoints;
    }

    if (m_TmpIpoints != NULL)
    {
        delete[] m_TmpIpoints;
    }
    //erstmal zum testen hier
    m_thresh = 0.002f;
    m_octaves = 5;
    m_initsample = 2;
    m_DescriptorLength = 64;
}

void CSurfCL::CreateCLProgram(char* _Filepath, char* _KernelName, cl_program& _rProgram, cl_kernel& _rKernel )
{
    cl_int ErrorCode;

    //load the cl file
    std::fstream Instream(_Filepath, std::fstream::in | std::fstream::binary);
    assert(Instream.is_open());

    //get the length
    Instream.seekg(0, std::fstream::end);
    size_t  FileSize = (size_t)Instream.tellg();
    Instream.seekg(0, std::fstream::beg);

    //read the sourcecode
    char* SourceCode = new char[FileSize + 1];
    Instream.read(SourceCode, FileSize);
    SourceCode[FileSize] = '\0';
    Instream.close();

    const char* src = const_cast<const char*>(SourceCode);

    _rProgram = clCreateProgramWithSource(m_Context, 1, &src, &FileSize , &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

    ErrorCode = clBuildProgram(_rProgram, 1, &m_DeviceID, "", NULL, NULL);

    //falls ein Fehler auftritt wird er ausgegeben. klappt bisher nur bei NVidia :(
    if( ErrorCode != 0)
    {
        size_t outSize = 0;
        int nestedErrorCode;
        nestedErrorCode= clGetProgramBuildInfo (_rProgram, m_DeviceID, CL_PROGRAM_BUILD_LOG, 0, 0, &outSize);
        assert(nestedErrorCode == CL_SUCCESS);

        char* log = new char[outSize];
        nestedErrorCode= clGetProgramBuildInfo (_rProgram, m_DeviceID, CL_PROGRAM_BUILD_LOG, outSize, log, NULL);
        assert(nestedErrorCode == CL_SUCCESS);
        std::cout<<log;
        delete[] log;
    }
    assert(ErrorCode == CL_SUCCESS);

    _rKernel = clCreateKernel(_rProgram, _KernelName, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

    delete[] SourceCode;
}

cl_int CSurfCL::GetFirstDevice(cl_device_id& _rID)
{
    cl_int errorCode;

    //gets the first platform
    cl_platform_id firstPlatform;
    errorCode = clGetPlatformIDs(1, &firstPlatform, NULL);
    if(errorCode != CL_SUCCESS)
    {
        return errorCode;
    }

    //gets the first device
    cl_device_id firstDevice;
    errorCode = clGetDeviceIDs(firstPlatform, CL_DEVICE_TYPE_ALL , 1, &firstDevice, NULL);    
    if(errorCode != CL_SUCCESS)
    {
        return errorCode;
    }

    _rID = firstDevice;
    return errorCode;
}


void CSurfCL::PrintCLDevices()
{
    cl_int errorCode;

    std::cout<<"init CL"<<std::endl;

    cl_uint numPlatforms = 0;
    errorCode = clGetPlatformIDs(0,0,&numPlatforms);
    if(errorCode != CL_SUCCESS)
    {
        std::cout<<"clGetPlatformIDs failed. Errorcode: " << errorCode<<std::endl;
        return;
    }

    if(numPlatforms > 0)
    {
        cl_platform_id* platformIDs = new cl_platform_id[numPlatforms];
        errorCode = clGetPlatformIDs(numPlatforms,platformIDs,0);
        for(unsigned int i = 0; i < numPlatforms; ++i)
        {
            PrintPlattformInfo(platformIDs[i]);
        }
        delete[] platformIDs;
    }
}

void CSurfCL::PrintPlattformInfo(cl_platform_id& _rID)
{
    const size_t buffLen = 1024;
    char buff[buffLen];
    size_t len;

    clGetPlatformInfo(_rID, CL_PLATFORM_NAME, buffLen, buff, &len);
    std::cout << "CL_PLATFORM_NAME: \t" << std::endl << buff << std::endl << std::endl;

    clGetPlatformInfo(_rID, CL_PLATFORM_VENDOR, buffLen, buff, &len);
    std::cout << "CL_PLATFORM_VENDOR: \t" << std::endl << buff << std::endl << std::endl;

    clGetPlatformInfo(_rID, CL_PLATFORM_VERSION, buffLen, buff, &len);
    std::cout << "CL_PLATFORM_VERSION: \t" << std::endl << buff << std::endl << std::endl;

    clGetPlatformInfo(_rID, CL_PLATFORM_PROFILE, buffLen, buff, &len);
    std::cout << "CL_PLATFORM_PROFILE: \t" << std::endl << buff << std::endl << std::endl;

    clGetPlatformInfo(_rID, CL_PLATFORM_EXTENSIONS, buffLen, buff, &len);
    std::cout << "CL_PLATFORM_EXTENSIONS: \t" << std::endl << buff << std::endl;

    std::cout << std::endl << std::endl;

    cl_uint deviceCount;
    clGetDeviceIDs(_rID, CL_DEVICE_TYPE_ALL, 0, 0, &deviceCount);

    cl_device_id* deviceIDs = new cl_device_id[deviceCount];
    clGetDeviceIDs(_rID, CL_DEVICE_TYPE_ALL, deviceCount, deviceIDs, &deviceCount);

    for(unsigned int i = 0; i < deviceCount; ++i)
    {
        PrintDeviceInfo(deviceIDs[i]);
    }
}


void CSurfCL::PrintDeviceInfo(cl_device_id& _rID)
{
    std::cout << "CL_DEVICE:" << std::endl << std::endl;
    const size_t buffLen = 1024;
    char buff[buffLen];
    size_t len;


    clGetDeviceInfo(_rID, CL_DEVICE_NAME, 0, 0, &len);
    clGetDeviceInfo(_rID, CL_DEVICE_NAME, len, buff, &len);
    std::cout << "CL_DEVICE_NAME" << std::endl << buff << std::endl << std::endl;

    clGetDeviceInfo(_rID, CL_DEVICE_TYPE, 0, 0, &len);
    clGetDeviceInfo(_rID, CL_DEVICE_TYPE, len, buff, &len);
    std::cout << "CL_DEVICE_TYPE(S): \t" << std::endl;
    if( buff[0] & CL_DEVICE_TYPE_CPU)
    {
        std::cout << "CL_DEVICE_TYPE_CPU" << std::endl << std::endl;
    }
    if( buff[0] & CL_DEVICE_TYPE_GPU)
    {
        std::cout << "CL_DEVICE_TYPE_GPU" << std::endl << std::endl;
    }
    if( buff[0] & CL_DEVICE_TYPE_ACCELERATOR)
    {
        std::cout << "CL_DEVICE_TYPE_ACCELERATOR" << std::endl << std::endl;
    }

    cl_bool boolresult;
    cl_uint result;
    cl_uint longresult;
    size_t size_tresult;

    clGetDeviceInfo(_rID, CL_DEVICE_AVAILABLE, sizeof(boolresult), &boolresult, 0);
    std::cout << "CL_DEVICE_AVAILABLE:\t";
    if(boolresult)
    {
        std::cout << "TRUE" << std::endl;
    }
    else
    {
        std::cout << "FALSE" << std::endl;
    }

    clGetDeviceInfo(_rID, CL_DEVICE_COMPILER_AVAILABLE, sizeof(boolresult), &boolresult, 0);
    std::cout << "CL_DEVICE_COMPILER_AVAILABLE:\t";
    if(boolresult)
    {
        std::cout << "TRUE" << std::endl;
    }
    else
    {
        std::cout << "FALSE" << std::endl;
    }

    //CL_DEVICE_DOUBLE_FP_CONFIG

    clGetDeviceInfo(_rID, CL_DEVICE_ENDIAN_LITTLE, sizeof(boolresult), &boolresult, 0);
    std::cout << "CL_DEVICE_ENDIAN_LITTLE:\t";
    if(boolresult)
    {
        std::cout << "TRUE" << std::endl;
    }
    else
    {
        std::cout << "FALSE" << std::endl;
    }

    clGetDeviceInfo(_rID, CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(boolresult), &boolresult, 0);
    std::cout << "CL_DEVICE_ERROR_CORRECTION_SUPPORT:\t";
    if(boolresult)
    {
        std::cout << "TRUE" << std::endl;
    }
    else
    {
        std::cout << "FALSE" << std::endl;
    }

    //CL_DEVICE_EXECUTION_CAPABILITIES
    //CL_DEVICE_EXTENSIONS
    //CL_DEVICE_GLOBAL_MEM_CACHE_SIZE
    //CL_DEVICE_GLOBAL_MEM_CACHE_TYPE
    //CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE

    clGetDeviceInfo(_rID, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(longresult), &longresult, 0);
    std::cout << "CL_DEVICE_GLOBAL_MEM_SIZE:\t" << longresult << std::endl;

    //CL_DEVICE_HALF_FP_CONFIG
    //CL_DEVICE_IMAGE...

    clGetDeviceInfo(_rID, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(result), &result, 0);
    std::cout << "CL_DEVICE_MAX_CLOCK_FREQUENCY:\t" << result << std::endl;

    clGetDeviceInfo(_rID, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(result), &result, 0);
    std::cout << "CL_DEVICE_MAX_COMPUTE_UNITS:\t" << result << std::endl;

    clGetDeviceInfo(_rID, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(result), &result, 0);
    std::cout << "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:\t" << result << std::endl;

    clGetDeviceInfo(_rID, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(result), &result, 0);
    std::cout << "CL_DEVICE_MAX_WORK_GROUP_SIZE:\t" << result << std::endl;

    clGetDeviceInfo(_rID, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(result), &result, 0);
    std::cout << "CL_DEVICE_MAX_WORK_ITEM_SIZES:\t" << result << std::endl;

    clGetDeviceInfo(_rID, CL_DEVICE_MAX_CONSTANT_ARGS, sizeof(result), &result, 0);
    std::cout << "CL_DEVICE_MAX_CONSTANT_ARGS:\t" << result << std::endl;

    clGetDeviceInfo(_rID, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(longresult), &longresult, 0);
    std::cout << "CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE:\t" << longresult << std::endl;

    clGetDeviceInfo(_rID, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(longresult), &longresult, 0);
    std::cout << "CL_DEVICE_MAX_MEM_ALLOC_SIZE:\t" << longresult << std::endl;

    clGetDeviceInfo(_rID, CL_DEVICE_MAX_PARAMETER_SIZE, sizeof(size_tresult), &size_tresult, 0);
    std::cout << "CL_DEVICE_MAX_PARAMETER_SIZE:\t" << size_tresult << std::endl;

    //....

    clGetDeviceInfo(_rID, CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(size_tresult), &size_tresult, 0);
    std::cout << "CL_DEVICE_PROFILING_TIMER_RESOLUTION:\t" << size_tresult << " nanosec "<< std::endl;

    //...

    clGetDeviceInfo(_rID, CL_DEVICE_VERSION, 0, 0, &len);
    clGetDeviceInfo(_rID, CL_DEVICE_VERSION, len, buff, &len);
    std::cout << "CL_DEVICE_VERSION" << std::endl << buff << std::endl << std::endl;

    clGetDeviceInfo(_rID, CL_DRIVER_VERSION, 0, 0, &len);
    clGetDeviceInfo(_rID, CL_DRIVER_VERSION, len, buff, &len);
    std::cout << "CL_DRIVER_VERSION" << std::endl << buff << std::endl << std::endl;
}


int CSurfCL::GetDeviceCount()
{
    cl_int errorCode;
    unsigned int deviceCount = 0;

    cl_uint numPlatforms = 0;
    errorCode = clGetPlatformIDs(0,0,&numPlatforms);

    if(errorCode == CL_SUCCESS && numPlatforms > 0)
    {
        cl_platform_id* platformIDs = new cl_platform_id[numPlatforms];
        errorCode = clGetPlatformIDs(numPlatforms,platformIDs,0);

        for(unsigned int i = 0; i < numPlatforms; ++i)
        {
            cl_uint plattformDeviceCount = 0;
            clGetDeviceIDs(platformIDs[i], CL_DEVICE_TYPE_ALL,0,NULL,&plattformDeviceCount);
            deviceCount += plattformDeviceCount;
        }
        delete[] platformIDs;
    }

    return deviceCount;
}


void CSurfCL::GetDevices(CCLDevice* _pDevices)
{
    cl_int errorCode;

    cl_uint numPlatforms = 0;
    errorCode = clGetPlatformIDs(0,0,&numPlatforms);

    int targetIndex = 0;

    if(errorCode == CL_SUCCESS && numPlatforms > 0)
    {
        cl_platform_id* platformIDs = new cl_platform_id[numPlatforms];
        errorCode = clGetPlatformIDs(numPlatforms,platformIDs,0);

        for(unsigned int i = 0; i < numPlatforms; ++i)
        {
            cl_uint plattformDeviceCount = 0;
            clGetDeviceIDs(platformIDs[i], CL_DEVICE_TYPE_ALL,0,NULL,&plattformDeviceCount);
            cl_device_id* ids = new cl_device_id[plattformDeviceCount];

            clGetDeviceIDs(platformIDs[i], CL_DEVICE_TYPE_ALL, plattformDeviceCount, ids, &plattformDeviceCount);

            for(cl_uint j = 0; j < plattformDeviceCount; j++)
            {
                _pDevices[targetIndex] = ids[targetIndex];
                targetIndex++;
            }
            delete[] ids;
        }
        delete[] platformIDs;
    }
}

void CSurfCL::TestCL()
{
    cl_int ErrorCode;

    size_t count = 1;
    float inVal = 2;
    //write in buffer
    ErrorCode = clEnqueueWriteBuffer(m_CommandQueue, m_TestBuffer, CL_FALSE, 0, sizeof(cl_float) * count, &inVal, 0, NULL, NULL);
    assert(ErrorCode == CL_SUCCESS);

    //set the buffer as argument
    ErrorCode = clSetKernelArg(m_TestKernel, 0, sizeof(cl_mem), &m_TestBuffer);
    assert(ErrorCode == CL_SUCCESS);

    //run and block during finished
    ErrorCode = clEnqueueNDRangeKernel(m_CommandQueue, m_TestKernel, 1, NULL, &count, &count, 0, NULL, NULL);
    assert(ErrorCode == CL_SUCCESS);

    float outVal = 0;
    //read
    ErrorCode = clEnqueueReadBuffer(m_CommandQueue, m_TestBuffer, CL_TRUE, 0, sizeof(cl_float) * count, &outVal, 0, NULL, NULL);
    assert(ErrorCode == CL_SUCCESS);

    std::cout << outVal << std::endl;
    assert(ErrorCode == CL_SUCCESS);
}

void CSurfCL::ResizeBuffer( int _Width, int _Height, EPixelFormat _Format)
{
    m_PixelFormat = _Format;
    cl_int ErrorCode;

    

    if(m_Width != _Width || m_Height != _Height) //always true at the first run
    {
        m_Width = _Width;
        m_Height = _Height;

        //IntegralImage
        if(m_IntegralImageInput != NULL) 
        {
            ErrorCode = clReleaseMemObject(m_IntegralImageInput);
            assert(ErrorCode == CL_SUCCESS);  
        }

        if( m_IntegralImageBuffer != NULL)
        {
            ErrorCode = clReleaseMemObject(m_IntegralImageBuffer);
            assert(ErrorCode == CL_SUCCESS);  
        }

        int PixelSize = m_PixelFormat;
        if( m_UseImages == CL_TRUE)
        {
            cl_image_format imgFormat;
            imgFormat.image_channel_order = CL_R;
            imgFormat.image_channel_data_type = CL_UNSIGNED_INT8;

            m_IntegralImageInput = clCreateImage2D(
                m_Context, 
                CL_MEM_READ_ONLY, 
                &imgFormat, 
                m_Width * PixelSize, 
                m_Height, 
                0, 
                0, 
                &ErrorCode);

            m_IntegralImageBuffer = clCreateBuffer(m_Context, CL_MEM_READ_ONLY, sizeof(cl_float) * m_Height * m_Width, NULL, &ErrorCode);
        }
        else
        {
            m_IntegralImageInput = clCreateBuffer(m_Context, CL_MEM_READ_ONLY, sizeof(cl_uchar) * m_Height * m_Width * PixelSize, NULL, &ErrorCode);
        }

        assert(ErrorCode == CL_SUCCESS);    

        if(m_IntegralImageOutput != NULL) 
        {
            ErrorCode = clReleaseMemObject(m_IntegralImageOutput);
            assert(ErrorCode == CL_SUCCESS);
        }

        if( m_UseImages == CL_TRUE)
        {
            cl_image_format imgOutFormat;
            imgOutFormat.image_channel_order = CL_R;
            imgOutFormat.image_channel_data_type = CL_FLOAT;

            m_IntegralImageOutput = clCreateImage2D(
                m_Context, 
                CL_MEM_READ_WRITE, 
                &imgOutFormat, 
                m_Width, 
                m_Height, 
                0, 
                0, 
                &ErrorCode);
        }
        else
        {
            m_IntegralImageOutput = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, sizeof(cl_float) * m_Height * m_Width, NULL, &ErrorCode);
        }
        assert(ErrorCode == CL_SUCCESS);

        //Hessian
        size_t count = m_Height * m_Width;

        if(m_HessianIpointsBuffer != NULL) 
        {
            ErrorCode = clReleaseMemObject(m_HessianIpointsBuffer);
            assert(ErrorCode == CL_SUCCESS);
        }
        m_HessianIpointsBuffer = clCreateBuffer(m_Context, CL_MEM_WRITE_ONLY, sizeof(SIpointSmall) * count, NULL, &ErrorCode);
        assert(ErrorCode == CL_SUCCESS);

        if(m_HessianRBuffer != NULL) 
        {
            ErrorCode =  clReleaseMemObject(m_HessianRBuffer);
            assert(ErrorCode == CL_SUCCESS);
        }

        if( m_UseImages == CL_TRUE)
        {
            cl_image_format imgOutFormat;
            imgOutFormat.image_channel_order = CL_R;
            imgOutFormat.image_channel_data_type = CL_FLOAT;

            m_HessianRBuffer = clCreateImage2D(
                m_Context, 
                CL_MEM_READ_WRITE, 
                &imgOutFormat, 
                m_Width, 
                m_Height * countresponselayers, 
                0, 
                0, 
                &ErrorCode);
        }
        else
        {
            m_HessianRBuffer = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, 
                sizeof(cl_float) * count * countresponselayers, NULL, &ErrorCode);
        }

        assert(ErrorCode == CL_SUCCESS);

        if(m_HessianLBuffer != NULL) 
        {
            ErrorCode =  clReleaseMemObject(m_HessianLBuffer);
            assert(ErrorCode == CL_SUCCESS);
        }

		if( m_UseImages == CL_TRUE)
		{
			cl_image_format imgOutFormat;
			imgOutFormat.image_channel_order = CL_R;
			imgOutFormat.image_channel_data_type = CL_UNSIGNED_INT8;

			m_HessianLBuffer = clCreateImage2D(
				m_Context, 
				CL_MEM_READ_WRITE, 
				&imgOutFormat, 
				m_Width, 
				m_Height * countresponselayers, 
				0, 
				0, 
				&ErrorCode);
		}
		else
		{
			m_HessianLBuffer = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, 
				sizeof(unsigned char) * count * countresponselayers, NULL, &ErrorCode);
			assert(ErrorCode == CL_SUCCESS);
		}

        // Responselayers brauchen ja nur einmal berechnet zu werden
        CalcResponseLayers();

        if (m_TmpIpoints != NULL)
        {
            delete[] m_TmpIpoints;
        }
        m_TmpIpoints = new SIpointSmall[m_Width * m_Height];

        if (m_IPointsCount) delete[] m_IPointsCount;

        m_IPointsCount = new unsigned int[m_Height];
        assert(m_IPointsCount != NULL);

        if(m_IPointsCount != NULL)
        {
            clReleaseMemObject(m_IPointsCountBuff);
        }

        m_IPointsCountBuff = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, 
            sizeof(cl_uint) * m_Height, NULL, &ErrorCode); 
        assert(ErrorCode == CL_SUCCESS);
    }
}

void CSurfCL::ReadBitmap(char* _Path, int& _rWidth, int& _rHeight , unsigned char** _pImageData)
{
    std::fstream Instream(_Path, std::fstream::in | std::fstream::binary);

    assert(Instream.is_open());

    char MagicNumber[2];
    Instream.read(&MagicNumber[0],2);
    assert(MagicNumber[0] == 'B' && MagicNumber[1] == 'M');

    Instream.seekg(18);
    int width;
    Instream.read((char*)&width,sizeof(width));

    int height;
    Instream.read((char*)&height,sizeof(height));

    unsigned char* pixels = new unsigned char[height * width * 3];
    //skip header
    Instream.seekg(54);

    int add = 0;
    if( (width * 3)%4 != 0 )
        add = 4 - (width * 3) % 4;
    //Bild wird von oben nach unten durchlaufen
    //abgespeichert wird es von unten nach oben
    for( int y = height -1; y >=0; y--)
    {
        Instream.read((char*)&pixels[0] + y * width * 3, width * 3);
        size_t nextPos = (size_t)Instream.tellg();
        nextPos += add;
        Instream.seekg(nextPos);
    }

    //handle delete later

    _rWidth = width;
    _rHeight = height;
    *_pImageData = pixels;
}

void CSurfCL::CreateIntegralImage(unsigned char* _pImageData)
{
    cl_int ErrorCode;
    
    cl_uint PixelSize = m_PixelFormat;

    if(m_UseImages)
    {

        size_t origin[] = {0, 0, 0};
        size_t region[] = {m_Width * PixelSize, m_Height, 1};
        ErrorCode = clEnqueueWriteImage(m_CommandQueue, 
            m_IntegralImageInput, 
            CL_TRUE, 
            origin, 
            region, 
            0, 
            0, 
            &_pImageData[0], 
            0, NULL, NULL);

        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(m_IntegralImageKernel, 0, sizeof(cl_mem), &m_IntegralImageInput);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(m_IntegralImageKernel, 1, sizeof(cl_mem), &m_IntegralImageOutput);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(m_IntegralImageKernel, 2, sizeof(cl_mem), &m_IntegralImageBuffer);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(m_IntegralImageKernel, 3, sizeof(cl_uint), &PixelSize);
        assert(ErrorCode == CL_SUCCESS);


    }else
    {
        //Achtung: Hier war ein Fehler CL_FALSE schreibt den Buffer parallel, während das Programm weiter ausgeführt wird
        ErrorCode = clEnqueueWriteBuffer(m_CommandQueue, 
            m_IntegralImageInput, 
            CL_TRUE, 
            0, 
            sizeof(cl_uchar) * m_Width * m_Height * PixelSize, 
            &_pImageData[0], 
            0, NULL, NULL);

        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(m_IntegralImageKernel, 0, sizeof(cl_uint), &m_Height);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(m_IntegralImageKernel, 1, sizeof(cl_uint), &m_Width);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(m_IntegralImageKernel, 2, sizeof(cl_mem), &m_IntegralImageInput);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(m_IntegralImageKernel, 3, sizeof(cl_mem), &m_IntegralImageOutput);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(m_IntegralImageKernel, 4, sizeof(cl_uint), &PixelSize);
        assert(ErrorCode == CL_SUCCESS);
    }

    size_t workerCount = m_Width;
    if( m_Height > m_Width ) 
    {
        workerCount = m_Height;
    }

    if( workerCount > m_IntegralImageKernelWorkSize)
    {
        workerCount = m_IntegralImageKernelWorkSize;
    }
    ErrorCode = clEnqueueNDRangeKernel(m_CommandQueue, 
        m_IntegralImageKernel, 
        1, 
        NULL, 
        &workerCount, 
        &workerCount, 
        0, NULL, NULL);

    assert(ErrorCode == CL_SUCCESS);

}

void CSurfCL::CalcResponseLayers()
{
    // responselayer hier berechnen
    if (m_responselayers != NULL)
    {
        delete[] m_responselayers;
    }

    m_responselayers = new SResponseLayer[countresponselayers];
    int w = (m_Width / m_initsample);
    int h = (m_Height / m_initsample);
    int s = m_initsample;
    cl_int ErrorCode = CL_SUCCESS;

    //Oktave 1 enthŠlt Layer 0 - 3
    //filter -> FilterCore
    //Bsp.
    //	Layer 0   init_samples = 2  width = 100 height = 100
    //  -> grš§e des Layers: 50 x 50
    //  -> jeder 2. Pixel wird mit der Filtergrš§e 9 (  9 x 9 ) ausgelesen
    //Layer 1,2,3 mit unterschiedlichen Filtergrš§en (immer ungenauer)
    if (m_octaves >= 1)
    {
        m_responselayers[0].width = w; m_responselayers[0].height = h; m_responselayers[0].step = s; m_responselayers[0].filter = 9;
        m_responselayers[1].width = w; m_responselayers[1].height = h; m_responselayers[1].step = s; m_responselayers[1].filter = 15;
        m_responselayers[2].width = w; m_responselayers[2].height = h; m_responselayers[2].step = s; m_responselayers[2].filter = 21;
        m_responselayers[3].width = w; m_responselayers[3].height = h; m_responselayers[3].step = s; m_responselayers[3].filter = 27;
    }

    if (m_octaves >= 2)
    {
        m_responselayers[4].width = w / 2; m_responselayers[4].height = h / 2; m_responselayers[4].step = s * 2; m_responselayers[4].filter = 39;
        m_responselayers[5].width = w / 2; m_responselayers[5].height = h / 2; m_responselayers[5].step = s * 2; m_responselayers[5].filter = 51;
    }

    if (m_octaves >= 3)
    {
        m_responselayers[6].width = w / 4; m_responselayers[6].height = h / 4; m_responselayers[6].step = s * 4; m_responselayers[6].filter = 75;
        m_responselayers[7].width = w / 4; m_responselayers[7].height = h / 4; m_responselayers[7].step = s * 4; m_responselayers[7].filter = 99;
    }	
    if (m_octaves >= 4)
    {
        m_responselayers[8].width = w / 8; m_responselayers[8].height = h / 8; m_responselayers[8].step = s * 8; m_responselayers[8].filter = 147;
        m_responselayers[9].width = w / 8; m_responselayers[9].height = h / 8; m_responselayers[9].step = s * 8; m_responselayers[9].filter = 195;
    }

    if (m_octaves >= 5)
    {
        m_responselayers[10].width = w / 16; m_responselayers[10].height = h / 16; m_responselayers[10].step = s * 16; m_responselayers[10].filter = 291;
        m_responselayers[11].width = w / 16; m_responselayers[11].height = h / 16; m_responselayers[11].step = s * 16; m_responselayers[11].filter = 387;
    }

    ErrorCode = clEnqueueWriteBuffer(m_CommandQueue, m_HessianResponseBuffer, CL_TRUE, 0, sizeof(SResponseLayer) * countresponselayers, m_responselayers, 0, NULL, NULL);
    assert(ErrorCode == CL_SUCCESS);
}

int CSurfCL::Hessian()
{
    //printf("Starte schnellen Hessen\n");
    cl_int ErrorCode = CL_SUCCESS;

    size_t offset = 0;
    int Layercount = countresponselayers;

    // buffer fŸr die Responses 
	if(m_UseImages == CL_TRUE)
	{		
		ErrorCode = clSetKernelArg(m_ResponseKernel, 0, sizeof(cl_mem), &m_IntegralImageOutput);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 1, sizeof(cl_uint), &Layercount);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 2, sizeof(cl_mem), &m_HessianResponseBuffer);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 3, sizeof(cl_mem), &m_HessianRBuffer);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 4, sizeof(cl_mem), &m_HessianLBuffer);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 5, sizeof(cl_int) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 6, sizeof(cl_int) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 7, sizeof(cl_int) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 8, sizeof(cl_int) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 9, sizeof(cl_float) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
	}
	else
	{
		ErrorCode = clSetKernelArg(m_ResponseKernel, 0, sizeof(cl_uint), &m_Height);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 1, sizeof(cl_uint), &m_Width);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 2, sizeof(cl_uint), &Layercount);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 3, sizeof(cl_mem), &m_IntegralImageOutput);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 4, sizeof(cl_mem), &m_HessianResponseBuffer);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 5, sizeof(cl_mem), &m_HessianRBuffer);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 6, sizeof(cl_mem), &m_HessianLBuffer);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 7, sizeof(cl_int) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 8, sizeof(cl_int) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 9, sizeof(cl_int) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 10, sizeof(cl_int) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 11, sizeof(cl_float) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
	}
	if(m_UseImages == CL_TRUE)
	{		
		ErrorCode = clSetKernelArg(m_ResponseKernel, 0, sizeof(cl_mem), &m_IntegralImageOutput);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 1, sizeof(cl_uint), &Layercount);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 2, sizeof(cl_mem), &m_HessianResponseBuffer);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 3, sizeof(cl_mem), &m_HessianRBuffer);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 4, sizeof(cl_mem), &m_HessianLBuffer);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 5, sizeof(cl_int) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 6, sizeof(cl_int) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 7, sizeof(cl_int) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 8, sizeof(cl_int) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 9, sizeof(cl_float) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
	}
	else
	{
		ErrorCode = clSetKernelArg(m_ResponseKernel, 0, sizeof(cl_uint), &m_Height);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 1, sizeof(cl_uint), &m_Width);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 2, sizeof(cl_uint), &Layercount);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 3, sizeof(cl_mem), &m_IntegralImageOutput);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 4, sizeof(cl_mem), &m_HessianResponseBuffer);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 5, sizeof(cl_mem), &m_HessianRBuffer);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 6, sizeof(cl_mem), &m_HessianLBuffer);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 7, sizeof(cl_int) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 8, sizeof(cl_int) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 9, sizeof(cl_int) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 10, sizeof(cl_int) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_ResponseKernel, 11, sizeof(cl_float) * countresponselayers, NULL);
		assert(ErrorCode == CL_SUCCESS);
	}
    if(m_UseImages == CL_TRUE)
    {		
        ErrorCode = clSetKernelArg(m_ResponseKernel, 0, sizeof(cl_mem), &m_IntegralImageOutput);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 1, sizeof(cl_uint), &Layercount);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 2, sizeof(cl_mem), &m_HessianResponseBuffer);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 3, sizeof(cl_mem), &m_HessianRBuffer);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 4, sizeof(cl_mem), &m_HessianLBuffer);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 5, sizeof(cl_int) * countresponselayers, NULL);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 6, sizeof(cl_int) * countresponselayers, NULL);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 7, sizeof(cl_int) * countresponselayers, NULL);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 8, sizeof(cl_int) * countresponselayers, NULL);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 9, sizeof(cl_float) * countresponselayers, NULL);
        assert(ErrorCode == CL_SUCCESS);
    }
    else
    {
        ErrorCode = clSetKernelArg(m_ResponseKernel, 0, sizeof(cl_uint), &m_Height);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 1, sizeof(cl_uint), &m_Width);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 2, sizeof(cl_uint), &Layercount);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 3, sizeof(cl_mem), &m_IntegralImageOutput);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 4, sizeof(cl_mem), &m_HessianResponseBuffer);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 5, sizeof(cl_mem), &m_HessianRBuffer);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 6, sizeof(cl_mem), &m_HessianLBuffer);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 7, sizeof(cl_int) * countresponselayers, NULL);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 8, sizeof(cl_int) * countresponselayers, NULL);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 9, sizeof(cl_int) * countresponselayers, NULL);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 10, sizeof(cl_int) * countresponselayers, NULL);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_ResponseKernel, 11, sizeof(cl_float) * countresponselayers, NULL);
        assert(ErrorCode == CL_SUCCESS);
    }
    ErrorCode = clEnqueueNDRangeKernel(m_CommandQueue, m_ResponseKernel, 1, &offset, &m_ResponseKernelWorkSize, &m_ResponseKernelWorkSize, 0, NULL, NULL);
    assert(ErrorCode == CL_SUCCESS);

    //////// Parallelisieren der IPoints
    assert(m_IPointsCount != NULL);
    for (int i = 0; i < m_Height; i++) {
        m_IPointsCount[i] = 0;
    }

    ErrorCode = clEnqueueWriteBuffer(m_CommandQueue, m_IPointsCountBuff, CL_TRUE, 0, sizeof(cl_uint) * m_Height, m_IPointsCount, 0, NULL, NULL);
    assert(ErrorCode == CL_SUCCESS);


    int filter_map[20] = {0,1,2,3,1,3,4,5,3,5,6,7,5,7,8,9,7,9,10,11};
    int Count = 0;


    memset(m_TmpIpoints, -1, m_Width * m_Height);
    ErrorCode = clEnqueueWriteBuffer(m_CommandQueue, m_HessianIpointsBuffer, CL_TRUE, 0, sizeof(SIpointSmall) * m_Height * m_Width, m_TmpIpoints, 0, NULL, NULL);
    assert(ErrorCode == CL_SUCCESS);

    ErrorCode = clSetKernelArg(m_IPointsKernel, 3, sizeof(cl_uint), &m_Width);
    assert(ErrorCode == CL_SUCCESS);
    ErrorCode = clSetKernelArg(m_IPointsKernel, 4, sizeof(cl_uint), &m_Height);
    assert(ErrorCode == CL_SUCCESS);
    ErrorCode = clSetKernelArg(m_IPointsKernel, 5, sizeof(cl_uint), &m_thresh);
    assert(ErrorCode == CL_SUCCESS);
    ErrorCode = clSetKernelArg(m_IPointsKernel, 6, sizeof(cl_mem), &m_IPointsCountBuff);
    assert(ErrorCode == CL_SUCCESS);
    ErrorCode = clSetKernelArg(m_IPointsKernel, 7, sizeof(cl_mem), &m_HessianIpointsBuffer);
    assert(ErrorCode == CL_SUCCESS);
    ErrorCode = clSetKernelArg(m_IPointsKernel, 8, sizeof(cl_mem), &m_HessianResponseBuffer);
    assert(ErrorCode == CL_SUCCESS);
    ErrorCode = clSetKernelArg(m_IPointsKernel, 9, sizeof(cl_mem), &m_HessianRBuffer);
    assert(ErrorCode == CL_SUCCESS);
    ErrorCode = clSetKernelArg(m_IPointsKernel, 10, sizeof(cl_mem), &m_HessianLBuffer);
    assert(ErrorCode == CL_SUCCESS);

    for (int o = 0; o < m_octaves; ++o) {
        for (int j = 0; j <= 1; ++j) {
            unsigned int b = filter_map[4 * o + j];
            unsigned int m = filter_map[4 * o + j + 1];
            unsigned int t = filter_map[4 * o + j + 2];

            // hier losschicken
            // Index ist height
            ErrorCode = clSetKernelArg(m_IPointsKernel, 0, sizeof(cl_uint), &t);
            assert(ErrorCode == CL_SUCCESS);
            ErrorCode = clSetKernelArg(m_IPointsKernel, 1, sizeof(cl_uint), &m);
            assert(ErrorCode == CL_SUCCESS);
            ErrorCode = clSetKernelArg(m_IPointsKernel, 2, sizeof(cl_uint), &b);
            assert(ErrorCode == CL_SUCCESS);


            ErrorCode = clEnqueueNDRangeKernel(m_CommandQueue, m_IPointsKernel, 1, &offset, &m_IPointsKernelWorkSize, &m_IPointsKernelWorkSize, 0, NULL, NULL);
            assert(ErrorCode == CL_SUCCESS);
        }
    }

    // Wieviele Punkte wurden gefunden?
    ErrorCode = clEnqueueReadBuffer(m_CommandQueue, m_IPointsCountBuff, CL_TRUE, 0, 
        sizeof(cl_uint) * m_Height, m_IPointsCount, 0, NULL, NULL);
    assert(ErrorCode == CL_SUCCESS);

    for (int i = 0; i < m_Height; i++) {
        Count += m_IPointsCount[i];
    }

    // Buffer anlegen
    // nur wenn was gefunden
    if (Count > 0)
    {
        if(m_GetIpoints != NULL) 
        {
            ErrorCode = clReleaseMemObject(m_GetIpoints);
            assert(ErrorCode == CL_SUCCESS);
        }
        m_GetIpoints = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, 
            sizeof(SIpoint) * Count, NULL, &ErrorCode);
        assert(ErrorCode == CL_SUCCESS);

        // Parameter Ÿbergeben 
        ErrorCode = clSetKernelArg(m_GetIPointsKernel, 0, sizeof(cl_uint), &Count);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_GetIPointsKernel, 1, sizeof(cl_uint), &m_Height);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_GetIPointsKernel, 2, sizeof(cl_uint), &m_Width);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_GetIPointsKernel, 3, sizeof(cl_uint), &m_Height);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_GetIPointsKernel, 4, sizeof(cl_mem), &m_IPointsCountBuff);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_GetIPointsKernel, 5, sizeof(cl_mem), &m_HessianIpointsBuffer);
        assert(ErrorCode == CL_SUCCESS);
        ErrorCode = clSetKernelArg(m_GetIPointsKernel, 6, sizeof(cl_mem), &m_GetIpoints);
        assert(ErrorCode == CL_SUCCESS);

        // Ipoints ins Array kopieren lassen
        size_t oneWorker = 1;
        ErrorCode = clEnqueueNDRangeKernel(m_CommandQueue, m_GetIPointsKernel, 1, 0, &oneWorker, &oneWorker, 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);

        if (m_Ipoints != NULL) {
            delete[] m_Ipoints;
        }

        m_Ipoints = new SIpoint[Count];

        ErrorCode = clEnqueueReadBuffer(m_CommandQueue, m_GetIpoints, CL_TRUE, 0, 
            sizeof(SIpoint) * Count, m_Ipoints, 0, NULL, NULL);  
        assert(ErrorCode == CL_SUCCESS);

        clFinish(m_CommandQueue);

        m_IPointCount = Count;
    }

    return Count;
}

void CSurfCL::CalculateSurfDescriptor()
{
    //calculates descriptors and orrientation
    if(m_IPointCount == 0)
        return;
    cl_int ErrorCode = CL_SUCCESS;
    size_t maxWorkSize = m_SurfDescriptorKernelWorkSize;
    size_t offset = 0;
    unsigned int DescrLength = m_DescriptorLength;
    if(m_UseImages == CL_TRUE)
    {
        ErrorCode = clSetKernelArg(m_SurfDescriptorKernel, 0, sizeof(cl_mem), &m_IntegralImageOutput);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(m_SurfDescriptorKernel, 1, sizeof(cl_int), &m_Height);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(m_SurfDescriptorKernel, 2, sizeof(cl_int), &m_Width);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(m_SurfDescriptorKernel, 3, sizeof(cl_int), &m_IPointCount);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(m_SurfDescriptorKernel, 4, sizeof(cl_int), &DescrLength);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(m_SurfDescriptorKernel, 5, sizeof(cl_mem), &m_GetIpoints);
        assert(ErrorCode == CL_SUCCESS);
    }
    else
    {
        ErrorCode = clSetKernelArg(m_SurfDescriptorKernel, 0, sizeof(cl_int), &m_Height);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(m_SurfDescriptorKernel, 1, sizeof(cl_int), &m_Width);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(m_SurfDescriptorKernel, 2, sizeof(cl_int), &m_IPointCount);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(m_SurfDescriptorKernel, 3, sizeof(cl_int), &DescrLength);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(m_SurfDescriptorKernel, 4, sizeof(cl_mem), &m_IntegralImageOutput);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(m_SurfDescriptorKernel, 5, sizeof(cl_mem), &m_GetIpoints);
        assert(ErrorCode == CL_SUCCESS);
    }

    // OpenCL Kernel ausführen
    ErrorCode = clEnqueueNDRangeKernel(m_CommandQueue, m_SurfDescriptorKernel, 1, &offset, &maxWorkSize, &maxWorkSize, 0, NULL, NULL);
    assert(ErrorCode == CL_SUCCESS);

     ErrorCode = clEnqueueReadBuffer(m_CommandQueue, m_GetIpoints, CL_TRUE, 0, 
            sizeof(SIpoint) * m_IPointCount, m_Ipoints, 0, NULL, NULL);  
        assert(ErrorCode == CL_SUCCESS);

    clFinish(m_CommandQueue);
}

/*
int CSurfCL::Hessian()
{	
//erstmal zum testen hier
m_thresh = 0.0002;
m_octaves = 2;
m_initsample = 2;

cl_int ErrorCode = CL_SUCCESS;
unsigned int LayerCount = countresponselayers;
unsigned int *IPointCount = new unsigned int [1];
assert(IPointCount != NULL);

IPointCount[0] = 0;
unsigned int Count = 0;

assert(m_IPointsCountBuff != NULL);

ErrorCode = clEnqueueWriteBuffer(m_CommandQueue, m_IPointsCountBuff, CL_TRUE, 0, sizeof(cl_uint), IPointCount, 0, NULL, NULL);
assert(ErrorCode == CL_SUCCESS);

//set the buffer as argument fŸr die Responselayers
ErrorCode = clSetKernelArg(m_HessianKernel, 0, sizeof(cl_uint), &m_Height);
assert(ErrorCode == CL_SUCCESS);
ErrorCode = clSetKernelArg(m_HessianKernel, 1, sizeof(cl_uint), &m_Width);
assert(ErrorCode == CL_SUCCESS);
ErrorCode = clSetKernelArg(m_HessianKernel, 2, sizeof(cl_float), &m_thresh);
assert(ErrorCode == CL_SUCCESS);
ErrorCode = clSetKernelArg(m_HessianKernel, 3, sizeof(cl_uint), &m_octaves);
assert(ErrorCode == CL_SUCCESS);
ErrorCode = clSetKernelArg(m_HessianKernel, 4, sizeof(cl_uint), &m_initsample);
assert(ErrorCode == CL_SUCCESS);
ErrorCode = clSetKernelArg(m_HessianKernel, 5, sizeof(cl_uint), &LayerCount);
assert(ErrorCode == CL_SUCCESS);
ErrorCode = clSetKernelArg(m_HessianKernel, 6, sizeof(cl_mem), &m_IPointsCountBuff);
assert(ErrorCode == CL_SUCCESS);
ErrorCode = clSetKernelArg(m_HessianKernel, 7, sizeof(cl_mem), &m_IntegralImageOutput);
assert(ErrorCode == CL_SUCCESS);
ErrorCode = clSetKernelArg(m_HessianKernel, 8, sizeof(cl_mem), &m_HessianIpointsBuffer);
assert(ErrorCode == CL_SUCCESS);
ErrorCode = clSetKernelArg(m_HessianKernel, 9, sizeof(cl_mem), &m_HessianResponseBuffer);
assert(ErrorCode == CL_SUCCESS);
ErrorCode = clSetKernelArg(m_HessianKernel, 10, sizeof(cl_mem), &m_HessianRBuffer);
assert(ErrorCode == CL_SUCCESS);
ErrorCode = clSetKernelArg(m_HessianKernel, 11, sizeof(cl_mem), &m_HessianLBuffer);
assert(ErrorCode == CL_SUCCESS);


//run and block during finished
size_t offset = 0;
size_t scount = 1;
ErrorCode = clEnqueueNDRangeKernel(m_CommandQueue, m_HessianKernel, 1, &offset, &scount, &scount, 0, NULL, NULL);
assert(ErrorCode == CL_SUCCESS);

clFinish(m_CommandQueue);
// Auslesen
ErrorCode = clEnqueueReadBuffer(m_CommandQueue, m_IPointsCountBuff, CL_TRUE, 0, 
sizeof(cl_uint), IPointCount, 0, NULL, NULL);
assert(ErrorCode == CL_SUCCESS);

Count = IPointCount[0];
delete[] IPointCount;

if (m_Ipoints != NULL)
{
delete[] m_Ipoints;
}

m_Ipoints = new SIpoint[Count];

ErrorCode = clEnqueueReadBuffer(m_CommandQueue, m_HessianIpointsBuffer, CL_TRUE, 0, 
sizeof(SIpoint) * Count, m_Ipoints, 0, NULL, NULL);  
assert(ErrorCode == CL_SUCCESS);

return Count;
}

*/
void CSurfCL::DebugScanFromBitmap(char* _File,float _Threshold, int _Octaves, int _Initsample, bool _ExtendedDescriptor)
{
    int Height = 0;
    int Width = 0;
    unsigned char* ImgData;
    ReadBitmap(_File, Width, Height, &ImgData);
    ScanImage(Width, Height, ImgData, _Threshold, _Octaves, _Initsample, _ExtendedDescriptor, CSurfCL::BGR);
    delete[] ImgData;
}

void CSurfCL::DebugGetIntegralImage(float* result)
{    
    

	if(m_UseImages)
	{
		size_t origin[] = {0,0,0};
		size_t region[] = {m_Width,m_Height,1};
		cl_int ErrorCode = clEnqueueReadImage(m_CommandQueue, m_IntegralImageOutput, CL_TRUE, origin, region, 0, 0, result, 0, NULL, NULL);
		assert(ErrorCode == CL_SUCCESS);
	}else
	{
		size_t size = m_Height * m_Width;
		//read
		cl_int ErrorCode = clEnqueueReadBuffer(m_CommandQueue, m_IntegralImageOutput, CL_TRUE, 0, sizeof(cl_float) * size, result, 0, NULL, NULL);
		assert(ErrorCode == CL_SUCCESS);
	}

}

void CSurfCL::DebugPrintIntegralImage()
{
    std::cout<<"Debug IntegralImage:" << std::endl;
    float* matrix = new float[m_Height * m_Width];
    DebugGetIntegralImage(matrix);
    for( int y = 0; y < m_Height; y++)
    {
        for( int x = 0; x < m_Width; x++ )
        {
            std::cout << matrix[y*m_Width + x] << " \t";
        }

        std::cout << std::endl;
    }

    delete[] matrix;
}

void CSurfCL::ErrorHandler(const char * _Msg, const void * _Info, size_t cb, void * _UserData)
{
    std::cout << _Msg;
}

// Schnittstelle
void CSurfCL::ScanImage(int _Width, int _Height, unsigned char* _ImageData,
    float _Threshold, int _Octaves, int _Initsample, bool _ExtendedDescriptor, CSurfCL::EPixelFormat _Format)
{
    m_thresh = _Threshold;
    m_octaves = _Octaves;
    m_initsample = _Initsample;
    if(_ExtendedDescriptor)
    {
        m_DescriptorLength = 128;
    }else
    {
        m_DescriptorLength = 64;
    }

    ResizeBuffer(_Width, _Height, _Format);

    CreateIntegralImage(_ImageData);
    m_IPointCount = Hessian();
}
void CSurfCL::ScanImage24Bit(int _Width, int _Height, unsigned char* _ImageData, float _Threshold, int _Octaves, int _Initsample, bool _ExtendedDescriptor)
{
    ScanImage(_Width, _Height, _ImageData, _Threshold, _Octaves, _Initsample, _ExtendedDescriptor, CSurfCL::BGR);
}

void CSurfCL::ScanImage32Bit(int _Width, int _Height, unsigned char* _ImageData, float _Threshold, int _Octaves, int _Initsample, bool _ExtendedDescriptor)
{
    ScanImage(_Width, _Height, _ImageData, _Threshold, _Octaves, _Initsample, _ExtendedDescriptor, CSurfCL::BGRA);
}

void CSurfCL::ScanImageGreyscale(int _Width, int _Height, unsigned char* _ImageData, float _Threshold, int _Octaves, int _Initsample, bool _ExtendedDescriptor)
{
    ScanImage(_Width, _Height, _ImageData, _Threshold, _Octaves, _Initsample, _ExtendedDescriptor, CSurfCL::GREYSCALE);
}
void CSurfCL::DebugScanImage(int _Width, int _Height, unsigned char* _ImageData,
    float _Threshold, int _Octaves, int _Initsample, bool _ExtendedDescriptor)
{
    m_thresh = _Threshold;
    m_octaves = _Octaves;
    m_initsample = _Initsample;
    if(_ExtendedDescriptor)
    {
        m_DescriptorLength = 128;
    }else
    {
        m_DescriptorLength = 64;
    }

    m_IPointCount = Hessian();
}

int CSurfCL::GetIPointsCount()
{
    return m_IPointCount;
}

void CSurfCL::GetIPointsCopy(SIpoint &_destArray)
{
    SIpoint *ptr = &_destArray;
    for (int i = 0; i < m_IPointCount; ++i) {
        ptr[i] = m_Ipoints[i];
    }
}

SIpoint *CSurfCL::GetIPointsLink()
{
    return m_Ipoints;
}
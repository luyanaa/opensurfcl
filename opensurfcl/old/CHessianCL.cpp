#include "CHessianCL.h"
#include <iostream>
#include "math.h"


CHessianCL::CHessianCL(cl_context* _Context, cl_command_queue* _CommandQueue, cl_kernel* _HessianKernel,
				   float thresh, int octaves, int init_sample, CIntegralImageCL *img)
{
	// Variablen zuweisen
	m_thresh = thresh;
	m_octaves = octaves;
	m_initsample = init_sample;
	m_img = img;
	m_Context = _Context;
    m_CommandQueue = _CommandQueue;
    m_Kernel = _HessianKernel;
}

CHessianCL::~CHessianCL(void)
{

}

char *print_cl_errstring(cl_int err) {
    switch (err) {
        case CL_SUCCESS:                          return strdup("Success!");
        case CL_DEVICE_NOT_FOUND:                 return strdup("Device not found.");
        case CL_DEVICE_NOT_AVAILABLE:             return strdup("Device not available");
        case CL_COMPILER_NOT_AVAILABLE:           return strdup("Compiler not available");
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:    return strdup("Memory object allocation failure");
        case CL_OUT_OF_RESOURCES:                 return strdup("Out of resources");
        case CL_OUT_OF_HOST_MEMORY:               return strdup("Out of host memory");
        case CL_PROFILING_INFO_NOT_AVAILABLE:     return strdup("Profiling information not available");
        case CL_MEM_COPY_OVERLAP:                 return strdup("Memory copy overlap");
        case CL_IMAGE_FORMAT_MISMATCH:            return strdup("Image format mismatch");
        case CL_IMAGE_FORMAT_NOT_SUPPORTED:       return strdup("Image format not supported");
        case CL_BUILD_PROGRAM_FAILURE:            return strdup("Program build failure");
        case CL_MAP_FAILURE:                      return strdup("Map failure");
        case CL_INVALID_VALUE:                    return strdup("Invalid value");
        case CL_INVALID_DEVICE_TYPE:              return strdup("Invalid device type");
        case CL_INVALID_PLATFORM:                 return strdup("Invalid platform");
        case CL_INVALID_DEVICE:                   return strdup("Invalid device");
        case CL_INVALID_CONTEXT:                  return strdup("Invalid context");
        case CL_INVALID_QUEUE_PROPERTIES:         return strdup("Invalid queue properties");
        case CL_INVALID_COMMAND_QUEUE:            return strdup("Invalid command queue");
        case CL_INVALID_HOST_PTR:                 return strdup("Invalid host pointer");
        case CL_INVALID_MEM_OBJECT:               return strdup("Invalid memory object");
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:  return strdup("Invalid image format descriptor");
        case CL_INVALID_IMAGE_SIZE:               return strdup("Invalid image size");
        case CL_INVALID_SAMPLER:                  return strdup("Invalid sampler");
        case CL_INVALID_BINARY:                   return strdup("Invalid binary");
        case CL_INVALID_BUILD_OPTIONS:            return strdup("Invalid build options");
        case CL_INVALID_PROGRAM:                  return strdup("Invalid program");
        case CL_INVALID_PROGRAM_EXECUTABLE:       return strdup("Invalid program executable");
        case CL_INVALID_KERNEL_NAME:              return strdup("Invalid kernel name");
        case CL_INVALID_KERNEL_DEFINITION:        return strdup("Invalid kernel definition");
        case CL_INVALID_KERNEL:                   return strdup("Invalid kernel");
        case CL_INVALID_ARG_INDEX:                return strdup("Invalid argument index");
        case CL_INVALID_ARG_VALUE:                return strdup("Invalid argument value");
        case CL_INVALID_ARG_SIZE:                 return strdup("Invalid argument size");
        case CL_INVALID_KERNEL_ARGS:              return strdup("Invalid kernel arguments");
        case CL_INVALID_WORK_DIMENSION:           return strdup("Invalid work dimension");
        case CL_INVALID_WORK_GROUP_SIZE:          return strdup("Invalid work group size");
        case CL_INVALID_WORK_ITEM_SIZE:           return strdup("Invalid work item size");
        case CL_INVALID_GLOBAL_OFFSET:            return strdup("Invalid global offset");
        case CL_INVALID_EVENT_WAIT_LIST:          return strdup("Invalid event wait list");
        case CL_INVALID_EVENT:                    return strdup("Invalid event");
        case CL_INVALID_OPERATION:                return strdup("Invalid operation");
        case CL_INVALID_GL_OBJECT:                return strdup("Invalid OpenGL object");
        case CL_INVALID_BUFFER_SIZE:              return strdup("Invalid buffer size");
        case CL_INVALID_MIP_LEVEL:                return strdup("Invalid mip-map level");
        default:                                  return strdup("Unknown");
    }
}

int CHessianCL::getIPoints(void)
{
	cl_mem IpointsBuffer;
	cl_mem ResponseBuffer;
	cl_mem LBuffer;
	cl_mem RBuffer;
	
	cl_int ErrorCode;
	int Height = m_img->GetHeight();
	int Width = m_img->GetWidth();
	int count = m_img->GetHeight() * m_img->GetWidth();
	const int countresponselayers = 12;
	
	IpointsBuffer = clCreateBuffer(*m_Context, CL_MEM_READ_WRITE, sizeof(SIpoint) * count, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);
	
	ResponseBuffer = clCreateBuffer(*m_Context, CL_MEM_READ_WRITE, 
									sizeof(SResponseLayer) * countresponselayers, NULL, &ErrorCode);
    
    RBuffer = clCreateBuffer(*m_Context, CL_MEM_READ_WRITE, 
									 sizeof(cl_float) * count * countresponselayers, NULL, &ErrorCode);
    
    
    LBuffer = clCreateBuffer(*m_Context, CL_MEM_READ_WRITE, 
									sizeof(unsigned char) * count * countresponselayers, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);
	
    //set the buffer as argument
    ErrorCode = clSetKernelArg(*m_Kernel, 0, sizeof(cl_uint), &Height);
    assert(ErrorCode == CL_SUCCESS);
    ErrorCode = clSetKernelArg(*m_Kernel, 1, sizeof(cl_uint), &Width);
    assert(ErrorCode == CL_SUCCESS);
	ErrorCode = clSetKernelArg(*m_Kernel, 2, sizeof(cl_float), &m_thresh);
    assert(ErrorCode == CL_SUCCESS);
	ErrorCode = clSetKernelArg(*m_Kernel, 3, sizeof(cl_uint), &m_octaves);
    assert(ErrorCode == CL_SUCCESS);
	ErrorCode = clSetKernelArg(*m_Kernel, 4, sizeof(cl_uint), &m_initsample);
    assert(ErrorCode == CL_SUCCESS);
	ErrorCode = clSetKernelArg(*m_Kernel, 5, sizeof(cl_uint), &countresponselayers);
    assert(ErrorCode == CL_SUCCESS);
    ErrorCode = clSetKernelArg(*m_Kernel, 6, sizeof(cl_mem), m_img->GetBuffer());
    assert(ErrorCode == CL_SUCCESS);
    ErrorCode = clSetKernelArg(*m_Kernel, 7, sizeof(cl_mem), &IpointsBuffer);
    assert(ErrorCode == CL_SUCCESS);
	ErrorCode = clSetKernelArg(*m_Kernel, 8, sizeof(cl_mem), &ResponseBuffer);
    assert(ErrorCode == CL_SUCCESS);
	ErrorCode = clSetKernelArg(*m_Kernel, 9, sizeof(cl_mem), &RBuffer);
    assert(ErrorCode == CL_SUCCESS);
	ErrorCode = clSetKernelArg(*m_Kernel, 10, sizeof(cl_mem), &LBuffer);
    assert(ErrorCode == CL_SUCCESS);
	
	
    //run and block during finished
    size_t offset = 0;
	size_t scound = 1;
    ErrorCode = clEnqueueNDRangeKernel(*m_CommandQueue, *m_Kernel, 1, &offset, &scound, &scound, 0, NULL, NULL);
    assert(ErrorCode == CL_SUCCESS);
	
	
	// Auslesen
    SResponseLayer layers[countresponselayers];
	SIpoint *ipoints = new SIpoint[count];
	for(int i= 0; i < count; i++)
    {
		ipoints[i].x = 0.0f;
    }
	
	ErrorCode = clEnqueueReadBuffer(*m_CommandQueue, ResponseBuffer, CL_TRUE, 0, 
									sizeof(SResponseLayer) * countresponselayers, layers, 0, NULL, NULL);
	//printf("ErrCode: %s\n", print_cl_errstring(ErrorCode));
    assert(ErrorCode == CL_SUCCESS);

	
    float* responses = new float[count * countresponselayers];
    unsigned char* laplace = new unsigned char[count * countresponselayers];
    
	ErrorCode = clEnqueueReadBuffer(*m_CommandQueue, RBuffer, CL_TRUE, 0, 
									sizeof(cl_float) * count * countresponselayers, responses, 0, NULL, NULL);
	//printf("ErrCode: %s\n", print_cl_errstring(ErrorCode));
    assert(ErrorCode == CL_SUCCESS);   

    /*for(int i= 0; i < count * countresponselayers; i++)
    {
        if(responses[i] != 0)
			std::cout<<responses[i] << " ";
    }*/

	ErrorCode = clEnqueueReadBuffer(*m_CommandQueue, LBuffer, CL_TRUE, 0, 
									sizeof(unsigned char) * count * countresponselayers, laplace, 0, NULL, NULL);
    //printf("ErrCode: %s\n", print_cl_errstring(ErrorCode));
    assert(ErrorCode == CL_SUCCESS);

	/*for(int i= 0; i < count * countresponselayers; i++)
    {
        if(laplace[i] != 0)
        std::cout<<(unsigned int)laplace[i] << " ";
    }*/
	
	ErrorCode = clEnqueueReadBuffer(*m_CommandQueue, IpointsBuffer, CL_TRUE, 0, 
									sizeof(SIpoint) * count, ipoints, 0, NULL, NULL);  
	//printf("ErrCode: %s\n", print_cl_errstring(ErrorCode));
    assert(ErrorCode == CL_SUCCESS);
	
	printf("IPoints: %d \n", (int)responses[0]);
    /*for(int i= 0; i < count; i++)
    {
		if (ipoints[i].x != 0.0f)
		{
			printf("Found IPoint: x=%d, y=%d\n", (int)ipoints[i].x, (int)ipoints[i].y);
		}
    }*/
	
	delete[] ipoints;
	
	return (int)responses[0];//Matrix[0];
}


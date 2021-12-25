#pragma once
#ifdef __APPLE__
	#include <OpenCL/OpenCL.h>
#else
	#include <CL/cl.h>
#endif
#include <vector>
#include "CIntegralImageCL.h"

using namespace std;

//Struct für IPoint
typedef struct
{
	float x,y;
    float scale;
    float response;
    float orientation;
    int laplacian;
    int descriptorLength;
    float descriptor[64];
}  SIpoint;

// Struct für ResponseLayer
typedef struct
{
	int responses;
	int laplacian;
	int width;
	int height;
	int step;
	int filter;
}  SResponseLayer;


class CHessianCL
{
protected:
	float m_thresh;
	int m_octaves;
	int m_initsample;
	CIntegralImageCL *m_img;
	cl_context* m_Context;
    cl_command_queue* m_CommandQueue;
    cl_kernel* m_Kernel;
	
public:
	CHessianCL(cl_context* _Context, cl_command_queue* _CommandQueue, cl_kernel* _HessianKernel,
			 float thresh, int octaves, int init_sample, CIntegralImageCL *img);
	~CHessianCL(void);
	
	int getIPoints(void);
	
};

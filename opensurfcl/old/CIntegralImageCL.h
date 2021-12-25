#pragma once
#include <math.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <assert.h>

#ifdef __APPLE__
    #include <OpenCL/OpenCL.h>
#else
    #include <CL/cl.h>
#endif

class CIntegralImageCL
{
private:
    static const float cR;
    static const float cG;
    static const float cB;

public:

    cl_context* m_Context;
    cl_command_queue* m_CommandQueue;
    cl_kernel* m_Kernel;
    
    cl_mem m_InputBuffer;
    cl_mem m_OutputBuffer;

    bool m_ResyncNeeded;

    float** m_Matrix;
    int m_Width;
    int m_Height;

    cl_mem *GetBuffer();
    int GetWidth();
	int GetHeight();
	
    CIntegralImageCL(cl_context* _Context, cl_command_queue* _CommandQueue, cl_kernel* mIntegralImageKernel, char* file);
    CIntegralImageCL(cl_context* _Context, cl_command_queue* _CommandQueue, cl_kernel* _IntegralImageKernel, unsigned char* pixels, int height, int width);
    ~CIntegralImageCL();

    float*& operator[](int index)
    {
        if( m_ResyncNeeded )
        {
            ResyncResultBuffer();
        }
        return m_Matrix[index];
    }


    unsigned char* ReadBitmap(char* _Path);
    void FromImage(unsigned char* imageData);

    void ResyncResultBuffer();

    void Print()
    {
        if(m_ResyncNeeded)
            ResyncResultBuffer();
        for( int y = 0; y < m_Height; y++)
        {
            for( int x = 0; x < m_Width; x++ )
            {
                std::cout << m_Matrix[y][x] << " \t";
            }

            std::cout << std::endl;
        }
    }

    //float BoxIntegral(int row, int col, int rows, int cols);

  };
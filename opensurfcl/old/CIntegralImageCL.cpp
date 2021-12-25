#include "CIntegralImageCL.h"
#include <cmath>

const float CIntegralImageCL::cR(0.2989f);
const float CIntegralImageCL::cG(0.5870f);
const float CIntegralImageCL::cB(0.1140f);

CIntegralImageCL::CIntegralImageCL(cl_context* _Context, cl_command_queue* _CommandQueue, cl_kernel* _IntegralImageKernel, char* file)
{
    m_Context = _Context;
    m_CommandQueue = _CommandQueue;
    m_Kernel = _IntegralImageKernel;

    m_Matrix = NULL;
    m_ResyncNeeded = true;


    unsigned char* pixels = ReadBitmap(file);
    
    cl_int ErrorCode;
    
    m_InputBuffer = clCreateBuffer(*m_Context, CL_MEM_READ_ONLY, sizeof(cl_uchar) * m_Height * m_Width * 3, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);    
    
    m_OutputBuffer = clCreateBuffer(*m_Context, CL_MEM_READ_WRITE, sizeof(cl_float) * m_Height * m_Width, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

    FromImage(pixels);
    delete[] pixels;

    
    this->m_Matrix = new float*[m_Height];
    this->m_Matrix[0] = new float[m_Height * m_Width];
    for(int i = 1; i < m_Height; i++)
    {
        this->m_Matrix[i] = this->m_Matrix[i-1] + m_Width;
    }
    
}
CIntegralImageCL::CIntegralImageCL(cl_context* _Context, cl_command_queue* _CommandQueue, cl_kernel* _IntegralImageKernel, unsigned char* pixels, int height, int width)
{
    m_Context = _Context;
    m_CommandQueue = _CommandQueue;
    m_Kernel = _IntegralImageKernel;

    m_Height = height;
    m_Width = width;

    m_Matrix = NULL;
    m_ResyncNeeded = true;
    
    cl_int ErrorCode;
    
    m_InputBuffer = clCreateBuffer(*m_Context, CL_MEM_READ_ONLY, sizeof(cl_uchar) * m_Height * m_Width * 3, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);    
    
    m_OutputBuffer = clCreateBuffer(*m_Context, CL_MEM_READ_WRITE, sizeof(cl_float) * m_Height * m_Width, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

    FromImage(pixels);

    
    this->m_Matrix = new float*[m_Height];
    this->m_Matrix[0] = new float[m_Height * m_Width];
    for(int i = 1; i < m_Height; i++)
    {
        this->m_Matrix[i] = this->m_Matrix[i-1] + m_Width;
    }
    
}
int CIntegralImageCL::GetWidth()
{
	return m_Width;
}

int CIntegralImageCL::GetHeight()
{
	return m_Height;
}

CIntegralImageCL::~CIntegralImageCL()
{
    clReleaseMemObject(this->m_InputBuffer);
    clReleaseMemObject(this->m_OutputBuffer);
    if( this->m_Matrix != NULL)
    {
        delete[] this->m_Matrix[0];
        delete[] this->m_Matrix;
    }
}



unsigned char* CIntegralImageCL::ReadBitmap(char* _Path)
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

    for( int y = 0; y < height; y++)
    {
        Instream.read((char*)&pixels[0] + y * width * 3, width * 3);
        int nextPos = Instream.tellg();
        nextPos += add;
        Instream.seekg(nextPos);
    }

    //handle delete later
    
    this->m_Height = height;
    this->m_Width = width;

    return pixels;
}



void CIntegralImageCL::FromImage(unsigned char* imageData)
{
    cl_int ErrorCode;
    
    
    
    size_t size = m_Height * m_Width * 3;

    //write in buffer
    //Achtung: Hier war ein Fehler CL_FALSE schreibt den Buffer parallel, während das Programm weiter ausgeführt wird
    //Im Releasemode waren daher die Ergebnisse falsch, weil er das OpenCL Programm ausgeführt hat bevor der Buffer voll war
    //ErrorCode = clEnqueueWriteBuffer(*m_CommandQueue, m_InputBuffer, CL_FALSE, 0, sizeof(cl_uchar) * size, &imageData[0], 0, NULL, NULL);
    ErrorCode = clEnqueueWriteBuffer(*m_CommandQueue, m_InputBuffer, CL_TRUE, 0, sizeof(cl_uchar) * size, &imageData[0], 0, NULL, NULL);
    assert(ErrorCode == CL_SUCCESS);

    //set the buffer as argument
    

    ErrorCode = clSetKernelArg(*m_Kernel, 0, sizeof(cl_uint), &m_Height);
    assert(ErrorCode == CL_SUCCESS);

    ErrorCode = clSetKernelArg(*m_Kernel, 1, sizeof(cl_uint), &m_Width);
    assert(ErrorCode == CL_SUCCESS);

    ErrorCode = clSetKernelArg(*m_Kernel, 3, sizeof(cl_mem), &m_InputBuffer);
    assert(ErrorCode == CL_SUCCESS);

    ErrorCode = clSetKernelArg(*m_Kernel, 4, sizeof(cl_mem), &m_OutputBuffer);
    assert(ErrorCode == CL_SUCCESS);

    //run and block during finished 
    //ok ist mir jetz zu doof bei der neuen struktur suchen wir uns nen besseren weg um an die max worksize zu kommen ...
    const size_t maxWorkSize=1;
    size_t workerCount = m_Height;
    size_t offset = 0;
    

    
    //Aufteilen der Threads auf mehrere Aufrufe ( geht sicherlich geschickter... )
    float countMaxRatio = workerCount / (float)maxWorkSize;
    int maximumCalls = floor(countMaxRatio);
    for(int i = 0; i < maximumCalls; i++)
    {
        ErrorCode = clSetKernelArg(*m_Kernel, 2, sizeof(size_t), &offset);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clEnqueueNDRangeKernel(*m_CommandQueue, *m_Kernel, 1, NULL, &maxWorkSize, &maxWorkSize, 0, NULL, NULL);
        offset += maxWorkSize;
        assert(ErrorCode == CL_SUCCESS);
    }
    //ging bei der Rundung was verloren muss nochmal ein Aufruf mit weniger Threads gemacht werden
    bool additionalCall = countMaxRatio != maximumCalls;
    if( additionalCall )
    {
        size_t workerNeeded = workerCount % maxWorkSize;
        //muss ein vielfaches von 2 sein
        int power = ceil(log10f(workerNeeded) / log10f(2));
        workerNeeded = pow( (float)2, power);

        ErrorCode = clSetKernelArg(*m_Kernel, 2, sizeof(size_t), &offset);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clEnqueueNDRangeKernel(*m_CommandQueue, *m_Kernel, 1, NULL, &workerNeeded, &workerNeeded, 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);
    }

    
    
    

    m_ResyncNeeded = true;
}

cl_mem *CIntegralImageCL::GetBuffer()
{
	if(m_ResyncNeeded)
		ResyncResultBuffer();
	
	return &m_OutputBuffer;
}

void CIntegralImageCL::ResyncResultBuffer()
{
    cl_int ErrorCode;
    size_t size = m_Height * m_Width;
    //read
    ErrorCode = clEnqueueReadBuffer(*m_CommandQueue, m_OutputBuffer, CL_TRUE, 0, sizeof(cl_float) * size, m_Matrix[0], 0, NULL, NULL);
    assert(ErrorCode == CL_SUCCESS);

    m_ResyncNeeded = false;
}

//float CIntegralImage::BoxIntegral(int row, int col, int rows, int cols)
//{
//    // The subtraction by one for row/col is because row/col is inclusive.
//    int r1 = std::min(row, Height) - 1;
//    int c1 = std::min(col, Width) - 1;
//    int r2 = std::min(row + rows, Height) - 1;
//    int c2 = std::min(col + cols, Width) - 1;
//
//    float A = 0, B = 0, C = 0, D = 0;
//    if (r1 >= 0 && c1 >= 0) A = Matrix[r1][c1];
//    if (r1 >= 0 && c2 >= 0) B = Matrix[r1][c2];
//    if (r2 >= 0 && c1 >= 0) C = Matrix[r2][c1];
//    if (r2 >= 0 && c2 >= 0) D = Matrix[r2][c2];
//
//    return std::max(0.0f, A - B - C + D);
//}


/// This code is based on the orginal OpenSurf Implementation by Chris Evans!
/// http://code.google.com/p/opensurf1

#pragma once

//Für Releaseversion! Alle daten müssen dann direkt bei der .exe Datei liegen!
//#define DATA_DIRECTORY "./"

#ifdef __APPLE__
    #include <OpenCL/OpenCL.h>
#else
    #include <CL/cl.h>
#endif



#include "CCLDevice.h"
#include "SIPoint.h"


//CL_CALLBACK is only for ATI
//define for NVidia
#ifndef CL_CALLBACK
#define CL_CALLBACK
#endif
	//Struct für IPoint

class CSurfCL
{
public:
    static void PrintCLDevices(); 
    static void PrintPlattformInfo(cl_platform_id& _rID);
    static void PrintDeviceInfo(cl_device_id& _rID);
    static cl_int GetFirstDevice(cl_device_id& _rID);
    static int GetDeviceCount();
    static void GetDevices(CCLDevice* _pDevices);
    static char* GetVersion()
    {
        return "V0.1";
    }
private:
    //Hessian structs

    enum EPixelFormat
    {
        GREYSCALE = 1,
        BGR = 3,
        BGRA = 4
    };


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

    //global
    cl_device_id m_DeviceID;
    cl_context m_Context;
    cl_command_queue m_CommandQueue;

	cl_bool m_UseImages;

    
    //integral image
    cl_program m_IntegralImageProgram;
    cl_kernel m_IntegralImageKernel;    
    cl_mem m_IntegralImageInput;
    cl_mem m_IntegralImageOutput;
	cl_mem m_IntegralImageBuffer;
	size_t m_IntegralImageKernelWorkSize;
	
    //hessian
    static const int countresponselayers = 12;
	cl_program m_HessianProgram;
    cl_kernel m_HessianKernel;
	size_t m_HessianKernelWorkSize;
	cl_program m_ResponseProgram;
    cl_kernel m_ResponseKernel;
	size_t m_ResponseKernelWorkSize;
	cl_program m_IPointsProgram;
    cl_kernel m_IPointsKernel;
	cl_program m_GetIPointsProgram;
    cl_kernel m_GetIPointsKernel;
	size_t m_IPointsKernelWorkSize;
    cl_mem m_HessianIpointsBuffer;
	cl_mem m_HessianResponseBuffer;
	cl_mem m_HessianLBuffer;
	cl_mem m_HessianRBuffer;
	cl_mem m_IPointsCountBuff;
	cl_mem m_GetIpoints;

    //SurfDescriptor
    cl_program m_SurfDescriptorProgram;
    cl_kernel m_SurfDescriptorKernel;
	size_t m_SurfDescriptorKernelWorkSize;
	cl_mem m_DIpointBuffer;
	
    //test
    cl_program m_TestProgram;
    cl_kernel m_TestKernel;
    cl_mem m_TestBuffer;
    
    //current image properties    
    int m_Width;
    int m_Height;
    EPixelFormat m_PixelFormat;

    //current hessian properties
	float m_thresh;
	int m_octaves;
	int m_initsample;
    
    int m_DescriptorLength;

	// for saving the IPoints
	SIpoint *m_Ipoints;
	SIpointSmall *m_TmpIpoints;
	int m_IPointCount;
	unsigned int *m_IPointsCount;
	
	// the responselayers
	SResponseLayer *m_responselayers;

public:
    CSurfCL();
    CSurfCL(CCLDevice& device);
    ~CSurfCL(void);
    void TestCL();

    
    //int GetIPoints(unsigned char* _ImgData, int _Width, int _Height);
	
    inline int GetLastWidth()
    {
        return m_Width;
    }
    inline int GetLastHeight()
    {
        return m_Height;
    }
    
    //die Debugmethoden werden später per Makro entfernt in der Retailversion
    void DebugPrintIntegralImage();
    void DebugGetIntegralImage(float* result);
    void DebugResizeBuffer(int _Width, int _Height)
    {
        ResizeBuffer(_Width, _Height, CSurfCL::BGR);
    }

    void DebugCreateIntegralImage(unsigned char* imageData)
    {
        CreateIntegralImage(imageData);
    }

    static void DebugReadBitmap(char* _Path, int& _rWidth, int& _rHeight , unsigned char** _rImageData)
    {
        ReadBitmap(_Path, _rWidth, _rHeight , _rImageData);
    }

	void DebugScanFromBitmap(char* _File, float _Threshold, int _Octaves, int _Initsample, bool extendedDescriptor);
	void DebugScanImage(int _Width, int _Height, unsigned char* _ImageData,
								 float _Threshold, int _Octaves, int _Initsample, bool extendedDescriptor);
	static void ReadBitmap(char* _Path, int& _rWidth, int& _rHeight , unsigned char** _pImageData);
	void CreateIntegralImage(unsigned char* _pImageData);
	
	// Schnittstelle
	void CalculateSurfDescriptor();
    int GetIPointsCount();
	void GetIPointsCopy(SIpoint &_destArray);
	SIpoint *GetIPointsLink();

    
    void ScanImage24Bit(int _Width, int _Height, unsigned char* _ImageData, float _Threshold, int _Octaves, int _Initsample, bool _ExtendedDescriptor);
    void ScanImage32Bit(int _Width, int _Height, unsigned char* _ImageData, float _Threshold, int _Octaves, int _Initsample, bool _ExtendedDescriptor);
    void ScanImageGreyscale(int _Width, int _Height, unsigned char* _ImageData, float _Threshold, int _Octaves, int _Initsample, bool _ExtendedDescriptor);

	
private:
    void Init(cl_device_id& device);
    void CreateCLProgram(char* _Filepath, char* _KernelName, cl_program& _Program, cl_kernel& _Kernel );
    
    void ScanImage(int _Width, int _Height, unsigned char* _ImageData, float _Threshold, int _Octaves, int _Initsample, bool _ExtendedDescriptor, EPixelFormat format);

    
    void ResizeBuffer( int _Width, int _Height, EPixelFormat _Format);
    int Hessian();
    void SurfDescriptorParalel();
	void CalcResponseLayers();
    static void CL_CALLBACK ErrorHandler(const char * _pMsg, const void * _pInfo, size_t _Cb, void * _pUserData);

};

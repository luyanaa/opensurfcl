#include <CSurfCL.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include "microtime.h"
#include "math.h"

const unsigned int imageWidth = 1280;
const unsigned int imageHeight = 768;
const unsigned int traverseCount = 100;

unsigned char* imageData;
float* integralImageData;

//#define DATA_DIRECTORY ".\\"
#ifndef CL_CALLBACK
#define CL_CALLBACK
#endif
void InitCL( cl_context& _rContext, cl_device_id& _rDevice, cl_command_queue& _rCommandQueue);
void CreateCLProgram(char* _Filepath, char* _KernelName, cl_context _Context, cl_device_id _Device, cl_program& _rProgram, cl_kernel& _rKernel );

unsigned char* CreateImageData();
float* CreateIntegralImageData();
void PrintResults(char* benchName, microsec initialisationTime, microsec startTime, microsec endTime);
void CL_CALLBACK ErrorHandler(const char * _Msg, const void * _Info, size_t cb, void * _UserData);

void BenchCurrentVersion();
void BenchCarstensImage();
//IntegralImage Benchmarks
void BenchIntegralImageCPU();
void BenchRealImage();
void BenchIntegralImageCurrentVersion();

void BenchHessianCarstensImage();
void BenchHessianRealImage();

void BenchIntegralImageParallel();
void BenchIntegralImageParallelV2();
void BenchIntegralImageSequencial();
void BenchIntegralImageParallelTwoPartsH();
void BenchIntegralImageParallelTwoPartsV();
void BenchDescriptorsCarstensImage();
void BenchIntegralImageParallelV3_32Bit();
void BenchIntegralImageParallelV3_24Bit();
void BenchIntegralImageParallelV3_24Bit_();
void BenchIntegralImageParallelV3_24BitTwoParts();

void BenchHessianV2();
void BenchHessianV3();

    typedef struct
    {
	    int responses;
	    int laplacian;
	    int width;
	    int height;
	    int step;
	    int filter;
    }  SResponseLayer;

/*
Aktuelle Werte fürs Integralimage:
CPU: Intel Core 2 Quad 2,66 GHz (nur 1 Kern wird verwendet!!!!!)
GPU: Geforce 8800M
Kurzfassung:

1280 x 768          pixel
CPU                 0.0210198
GPU                 0.0213305

5120 x 5120         pixel
CPU                 0.0211955
GPU                 0.0423209

1280 x 768
IntegralImageCPU:
        benchmark duration:        2074248microsec
        initialisation duration:   7915microsec
        calculation duration:      2066333microsec
        image calculation duration:20663.3
        pixel calculation duration:0.0210198
IntegralImageCurrentVersion:
        benchmark duration:        2097932microsec
        initialisation duration:   1062microsec
        calculation duration:      2096870microsec
        image calculation duration:20968.7
        pixel calculation duration:0.0213305

5120 x 5120
IntegralImageCPU:
        benchmark duration:        55747702microsec
        initialisation duration:   184897microsec
        calculation duration:      55562805microsec
        image calculation duration:555628
        pixel calculation duration:0.0211955
IntegralImageCurrentVersion:
        benchmark duration:        111005294microsec
        initialisation duration:   63521microsec
        calculation duration:      110941773microsec
        image calculation duration:1.10942e+006
        pixel calculation duration:0.0423209

        first Image duration:      58802 microsec
        last Image duration:       1114210 microsec
*/
    

/* Laptop von Prof. Grimm
    GPU: Geforce FX 880M

    IntegralImageCPU:
         benchmark duration:        14050887microsec
         initialisation duration:   13microsec
         calculation duration:      14050874microsec
         image calculation duration:14050.9
         pixel calculation duration:0.0142933
IntegralImageCurrentVersion:
         benchmark duration:        19040596microsec
         initialisation duration:   23microsec
         calculation duration:      19040573microsec
         image calculation duration:19040.6
         pixel calculation duration:0.0193691

         first Image duration:      38313 microsec
         last Image duration:       18606 microsec
IntegralImageParallel:
         benchmark duration:        17210045microsec
         initialisation duration:   5microsec
         calculation duration:      17210040microsec
         image calculation duration:17210
         pixel calculation duration:0.017507
IntegralImageParallelTwoParts:
         benchmark duration:        11291954microsec
         initialisation duration:   8microsec
         calculation duration:      11291946microsec
         image calculation duration:11291.9
         pixel calculation duration:0.0114868
IntegralImageParallelTwoPartsV:
         benchmark duration:        11425730microsec
         initialisation duration:   6microsec
         calculation duration:      11425724microsec
         image calculation duration:11425.7s
         pixel calculation duration:0.0116228

*/
int main(int argc, char** argv)
{
	
    imageData = CreateImageData();
	integralImageData = CreateIntegralImageData();
	BenchIntegralImageParallelV2();
	//BenchIntegralImageParallelV3_24Bit();
	BenchIntegralImageParallelV3_32Bit();
	BenchIntegralImageCPU();
	BenchIntegralImageParallel();
	BenchIntegralImageParallelTwoPartsH();
	BenchIntegralImageParallelTwoPartsV();
	
	/*BenchHessianV3();
	BenchHessianV2();
	BenchCurrentVersion();*/
	//BenchCarstensImage();
	
	//BenchCarstensImage();
	/*BenchHessianV3();
	BenchHessianV2();
	BenchCurrentVersion();
	
	BenchIntegralImageParallel();*/
	/*BenchIntegralImageParallelV3_24Bit();
	BenchIntegralImageParallelV3_24BitTwoParts();

	BenchIntegralImageParallelV2();
	BenchIntegralImageParallelV3_32Bit();*/
	//BenchHessianCarstensImage();
	
	//BenchHessianRealImage
	//BenchRealImage();
	

 //   BenchCurrentVersion();
	//
    /*
    BenchIntegralImageParallelV2();
	BenchIntegralImageParallelV3_24Bit();
	BenchIntegralImageParallelV3_32Bit();
	BenchIntegralImageParallelTwoPartsH();
	BenchIntegralImageParallelTwoPartsV();
    BenchCurrentVersion();
	BenchDescriptorsCarstensImage();
	
	BenchIntegralImageCPU();
	BenchIntegralImageParallel();*/



 //   workItemCount = 256;
 //   std::cout<<"Workitemsize: " << workItemCount;
	//BenchDescriptorsCarstensImage();
 //   BenchCurrentVersion();
 //   BenchCarstensImage();
	//BenchIntegralImageCPU();
	//BenchIntegralImageParallel();
	//BenchIntegralImageParallelV2();
	//BenchIntegralImageParallelTwoPartsH();
	//BenchIntegralImageParallelTwoPartsV();
	//BenchIntegralImageParallelV3();

	//BenchIntegralImageCurrentVersion();
	//BenchIntegralImageCPU();
	//BenchIntegralImageCurrentVersion();
	//BenchCurrentVersion();
	//BenchCarstensImage();
	//BenchRealImage();
	//BenchIntegralImageParallelV2();

    /*BenchIntegralImageCPU();
    BenchIntegralImageCurrentVersion();
    BenchIntegralImageSequencial();
    BenchIntegralImageParallel();
    BenchIntegralImageParallelTwoPartsH();
    BenchIntegralImageParallelTwoPartsV();*/

    //BenchCurrentVersion();
    //BenchIntegralImageParallelImage();
    delete[] imageData;
	delete[] integralImageData;
    return 0;
}

unsigned char* CreateImageData()
{
    const int size = imageHeight * imageWidth * 3;
    unsigned char* imgdata = new unsigned char[size];
    for(int i = 0; i < size; i++)
    {
        imgdata[i] = 0;
    }

    for(int y = 0; y < imageHeight; y++)
    {
        for(int x = 0; x < imageWidth; x++)
        {
            int index = (y*imageWidth+x)*3;
            imgdata[index] = (unsigned char)(x * (255.0f/imageWidth));
            imgdata[index+1] = (unsigned char)(y * (255.0f/imageHeight));
            imgdata[index+2] = (unsigned char)((x+y) * (255.0f/(imageWidth+imageHeight)));
        }
    }

    return imgdata;
}

float* CreateIntegralImageData()
{
    const float cR = 0.2989f;
    const float cG = 0.5870f;
    const float cB = 0.1140f;
    float* result = new float[imageWidth * imageHeight];

    for(int i = 0; i < traverseCount; i++)
    {
        float rowsum = 0;
        for (int x = 0; x < imageWidth; x++)
        {
            float r = imageData[x*3 + 2];
            float g = imageData[x*3 + 1];
            float b = imageData[x*3];
            rowsum += (cR * r + cG * g + cB * b) / 255.0f;
            result[x] = rowsum;
        }
        for (int y = 1; y < imageHeight; y++)
        {
            rowsum = 0;
            for (int x = 0; x < imageWidth; x++)
            {
                float r = imageData[(y * imageWidth + x) * 3 + 2];
                float g = imageData[(y * imageWidth + x) * 3 + 1];
                float b = imageData[(y * imageWidth + x) * 3];
            
                rowsum += (cR * r + cG * g + cB * b) / 255.0f;
            
                // integral image is rowsum + value above
                result[y * imageWidth + x] = rowsum + result[(y - 1) * imageWidth + x];
            }
        }
    }

	return result;
}

void InitCL( cl_context& _rContext, cl_device_id& _rDevice, cl_command_queue& _rCommandQueue)
{
    cl_int ErrorCode;
    
    cl_platform_id firstPlatform;
    ErrorCode = clGetPlatformIDs(1, &firstPlatform, NULL);
    assert(ErrorCode == CL_SUCCESS);

    //gets the first device
    ErrorCode = clGetDeviceIDs(firstPlatform, CL_DEVICE_TYPE_ALL , 1, &_rDevice, NULL);
    assert(ErrorCode == CL_SUCCESS);

    //create context
	_rContext = clCreateContext(0, 1, &_rDevice, ErrorHandler, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

    //create command queue
    _rCommandQueue = clCreateCommandQueue(_rContext, _rDevice, 0, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);
}
void CreateCLProgram(char* _Filepath, char* _KernelName, cl_context _Context, cl_device_id _Device, cl_program& _rProgram, cl_kernel& _rKernel )
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

    _rProgram = clCreateProgramWithSource(_Context, 1, &src, &FileSize , &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);
    
    ErrorCode = clBuildProgram(_rProgram, 1, &_Device, NULL, NULL, NULL);
    
    //falls ein Fehler auftritt wird er ausgegeben. klappt bisher nur bei NVidia :(
    if( ErrorCode != 0)
    {
        size_t outSize = 0;
        int nestedErrorCode;
        nestedErrorCode= clGetProgramBuildInfo (_rProgram, _Device, CL_PROGRAM_BUILD_LOG, 0, 0, &outSize);
        assert(nestedErrorCode == CL_SUCCESS);

	    char* log = new char[outSize];
        nestedErrorCode= clGetProgramBuildInfo (_rProgram, _Device, CL_PROGRAM_BUILD_LOG, outSize, log, NULL);
        assert(nestedErrorCode == CL_SUCCESS);
        std::cout<<log;
	    delete[] log;
    }
    assert(ErrorCode == CL_SUCCESS);

    _rKernel = clCreateKernel(_rProgram, _KernelName, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

    delete[] SourceCode;
}

void PrintResults(char* benchName, microsec initialisationTime, microsec startTime, microsec endTime)
{
    std::cout<<benchName<<":"<<std::endl;
    std::cout<<"\t"<<"benchmark duration:        "<< endTime - initialisationTime <<"microsec"<<std::endl;
    std::cout<<"\t"<<"initialisation duration:   "<< startTime - initialisationTime <<"microsec"<<std::endl;
    std::cout<<"\t"<<"calculation duration:      "<< endTime - startTime <<"microsec"<<std::endl;
    std::cout<<"\t"<<"image calculation duration:"<< (endTime - startTime) / (double)(traverseCount)<<std::endl;
    std::cout<<"\t"<<"pixel calculation duration:"<< (endTime - startTime) / (double)(imageHeight * imageWidth * traverseCount)<<std::endl;

}

void BenchHessianCarstensImage()
{
	//ignore compiletime
    CSurfCL surf = CSurfCL();
	
	
    //buffer initialisation
    microsec initialisationTime = GetMicro();
   
	
    //start
    microsec startTime = GetMicro();
	
    //extratests um Geschwindigkeitsverlust über die Zeit zu messen (evtl. durch schlechte Speicherverwaltung)
    microsec firstEndTime; //Zeit des Abschluss des ersten Bildes
    microsec lastStartTime;//Zeit des Startes des letzten Bildes
	
	int Height = 0;
    int Width = 0;
    unsigned char* ImgData;
    surf.ReadBitmap(DATA_DIRECTORY"/pic_carsten.bmp", Width, Height, &ImgData);
	surf.DebugResizeBuffer(Width, Height);
	surf.CreateIntegralImage(ImgData);
    
    
	
    for(int i = 0; i < traverseCount; i++)
    {
        if(i == traverseCount-1)
            lastStartTime = GetMicro();
        
		surf.DebugScanImage(Width, Height, ImgData, 0.0002f, 5, 2, false);
		
        if(i == 0)
            firstEndTime = GetMicro();
    }
	
    microsec endTime = GetMicro();
	
	delete[] ImgData;
    PrintResults("Hessian (Carstens Bild)", initialisationTime, startTime, endTime);
    
    std::cout<<std::endl;
    std::cout<<"\t"<<"first Image duration:      "<<firstEndTime - startTime<<" microsec"<<std::endl;
    std::cout<<"\t"<<"last Image duration:       "<<endTime - lastStartTime<<" microsec"<<std::endl;
}

void BenchHessianV2()
{
	int m_Width = imageWidth;
	int m_Height = imageHeight;
	cl_int ErrorCode;
    cl_context m_Context;
    cl_device_id device;
    cl_command_queue queue;
    cl_program program;
    cl_kernel m_ResponseKernel;
    cl_kernel m_IPointsKernel;
    cl_kernel m_GetIPointsKernel;
    InitCL(m_Context, device, queue);
    CreateCLProgram(DATA_DIRECTORY"/Hessian.cl","ResponseLayerParallelV2",m_Context,device, program, m_ResponseKernel);
    CreateCLProgram(DATA_DIRECTORY"/Hessian.cl","IPointsParallelV2",m_Context,device, program, m_IPointsKernel);
    CreateCLProgram(DATA_DIRECTORY"/Hessian.cl","GetIPoints",m_Context,device, program, m_GetIPointsKernel);

	
    microsec initialisationTime = GetMicro();

	size_t m_ResponseKernelWorkSize;
	clGetKernelWorkGroupInfo(m_ResponseKernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &m_ResponseKernelWorkSize, NULL);
	size_t m_IPointsKernelWorkSize;
	clGetKernelWorkGroupInfo(m_IPointsKernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &m_IPointsKernelWorkSize, NULL);
	size_t GetIPointsKernelIC;
	clGetKernelWorkGroupInfo(m_GetIPointsKernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &GetIPointsKernelIC, NULL);


	
    float m_thresh = 0.002f;
    int m_octaves = 5;
    int m_initsample = 2;
	int m_DescriptorLength = 64;
	
	
	const int countresponselayers = 12;

    
	cl_mem m_IntegralImageOutput = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, sizeof(cl_float) * m_Height * m_Width, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);
	ErrorCode = clEnqueueWriteBuffer(queue, m_IntegralImageOutput, CL_TRUE, 0, sizeof(cl_float) * m_Height * m_Width, &integralImageData[0], 0, NULL, NULL);
    assert(ErrorCode == CL_SUCCESS);

       

	cl_mem m_HessianResponseBuffer = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, 
        sizeof(SResponseLayer) * countresponselayers, NULL, &ErrorCode);
	assert(ErrorCode == CL_SUCCESS);

    cl_mem m_HessianIpointsBuffer = clCreateBuffer(m_Context, CL_MEM_WRITE_ONLY, sizeof(SIpointSmall) * m_Height * m_Width, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);


    cl_mem m_HessianRBuffer = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, 
        sizeof(cl_float) *  m_Height * m_Width * countresponselayers, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);


    cl_mem m_HessianLBuffer = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, 
        sizeof(unsigned char) *  m_Height * m_Width * countresponselayers, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);


    // Responselayers brauchen ja nur einmal berechnet zu werden
    SResponseLayer* m_responselayers = new SResponseLayer[countresponselayers];
	int w = (m_Width / m_initsample);
	int h = (m_Height / m_initsample);
	int s = m_initsample;


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

	ErrorCode = clEnqueueWriteBuffer(queue, m_HessianResponseBuffer, CL_TRUE, 0, sizeof(SResponseLayer) * countresponselayers, m_responselayers, 0, NULL, NULL);
	assert(ErrorCode == CL_SUCCESS);

	SIpointSmall* m_TmpIpoints = new SIpointSmall[m_Width * m_Height];



//Hessian
	microsec startTime = GetMicro();
	int m_IPointCount = 0;

	for(int i = 0; i < traverseCount; i++)
	{
		int Layercount = countresponselayers;

		// buffer fŸr die Responses 
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

		ErrorCode = clEnqueueNDRangeKernel(queue, m_ResponseKernel, 1, NULL, &m_ResponseKernelWorkSize, &m_ResponseKernelWorkSize, 0, NULL, NULL);
		assert(ErrorCode == CL_SUCCESS);
		//for ( int i = 0; i < countresponselayers; i++)
		//{

		//index -> index im Ziellayer (ein dimensionales Array)
		//ar und ac sind y und x Koordinate im Ziellayer


		// jetzt die Responses parallel berechnen


		//}

		//////// Parallelisieren der IPoints
		unsigned int *IPointCount = new unsigned int[m_IPointsKernelWorkSize];
		assert(IPointCount != NULL);
		for (int i = 0; i < m_IPointsKernelWorkSize; i++) {
			IPointCount[i] = 0;
		}

		
		cl_mem m_IpointsCount = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, 
			sizeof(cl_uint) * m_IPointsKernelWorkSize, NULL, &ErrorCode); 
		assert(ErrorCode == CL_SUCCESS);

		ErrorCode = clEnqueueWriteBuffer(queue, m_IpointsCount, CL_TRUE, 0, sizeof(cl_uint) * m_IPointsKernelWorkSize, IPointCount, 0, NULL, NULL);
		assert(ErrorCode == CL_SUCCESS);

		//???


		int filter_map[20] = {0,1,2,3,1,3,4,5,3,5,6,7,5,7,8,9,7,9,10,11};
		int Count = 0;

    
		memset(m_TmpIpoints, -1, m_Width * m_Height);
		ErrorCode = clEnqueueWriteBuffer(queue, m_HessianIpointsBuffer, CL_TRUE, 0, sizeof(SIpointSmall) * m_Height * m_Width, m_TmpIpoints, 0, NULL, NULL);
		assert(ErrorCode == CL_SUCCESS);
	
		ErrorCode = clSetKernelArg(m_IPointsKernel, 3, sizeof(cl_uint), &m_Width);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_IPointsKernel, 4, sizeof(cl_uint), &m_Height);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_IPointsKernel, 5, sizeof(cl_uint), &m_thresh);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_IPointsKernel, 6, sizeof(cl_mem), &m_IpointsCount);
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


				ErrorCode = clEnqueueNDRangeKernel(queue, m_IPointsKernel, 1, NULL, &m_IPointsKernelWorkSize, &m_IPointsKernelWorkSize, 0, NULL, NULL);
				assert(ErrorCode == CL_SUCCESS);
			}
		}

		// Wieviele Punkte wurden gefunden?
		ErrorCode = clEnqueueReadBuffer(queue, m_IpointsCount, CL_TRUE, 0, 
			sizeof(cl_uint) * m_IPointsKernelWorkSize, IPointCount, 0, NULL, NULL);
		assert(ErrorCode == CL_SUCCESS);

		for (int i = 0; i < m_IPointsKernelWorkSize; i++) {
			Count += IPointCount[i];
		}
	 
		// Buffer anlegen
		// nur wenn was gefunden
		if (Count > 0)
		{
			cl_mem m_GetIpoints = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, 
									 sizeof(SIpoint) * Count, NULL, &ErrorCode);
			assert(ErrorCode == CL_SUCCESS);
		
			// Parameter Ÿbergeben 
			ErrorCode = clSetKernelArg(m_GetIPointsKernel, 0, sizeof(cl_uint), &Count);
			assert(ErrorCode == CL_SUCCESS);
			ErrorCode = clSetKernelArg(m_GetIPointsKernel, 1, sizeof(cl_uint), &m_IPointsKernelWorkSize);
			assert(ErrorCode == CL_SUCCESS);
			ErrorCode = clSetKernelArg(m_GetIPointsKernel, 2, sizeof(cl_uint), &m_Width);
			assert(ErrorCode == CL_SUCCESS);
			ErrorCode = clSetKernelArg(m_GetIPointsKernel, 3, sizeof(cl_uint), &m_Height);
			assert(ErrorCode == CL_SUCCESS);
			ErrorCode = clSetKernelArg(m_GetIPointsKernel, 4, sizeof(cl_mem), &m_IpointsCount);
			assert(ErrorCode == CL_SUCCESS);
			ErrorCode = clSetKernelArg(m_GetIPointsKernel, 5, sizeof(cl_mem), &m_HessianIpointsBuffer);
			assert(ErrorCode == CL_SUCCESS);
			ErrorCode = clSetKernelArg(m_GetIPointsKernel, 6, sizeof(cl_mem), &m_GetIpoints);
			assert(ErrorCode == CL_SUCCESS);

			// Ipoints ins Array kopieren lassen
			size_t oneWorker = 1;
			ErrorCode = clEnqueueNDRangeKernel(queue, m_GetIPointsKernel, 1, 0, &oneWorker, &oneWorker, 0, NULL, NULL);
			assert(ErrorCode == CL_SUCCESS);

			SIpoint* m_Ipoints = new SIpoint[Count];

			ErrorCode = clEnqueueReadBuffer(queue, m_GetIpoints, CL_TRUE, 0, 
										sizeof(SIpoint) * Count, m_Ipoints, 0, NULL, NULL);  
			assert(ErrorCode == CL_SUCCESS);
		
			clFinish(queue);
	
			m_IPointCount = Count;
			clReleaseMemObject(m_GetIpoints);
		}
		clReleaseMemObject(m_IpointsCount);
		delete[] IPointCount;

	}
	
	microsec endTime = GetMicro();
    PrintResults("HessianV2", initialisationTime, startTime, endTime);

}

void BenchHessianV3()
{
	int m_Width = imageWidth;
	int m_Height = imageHeight;
	cl_int ErrorCode;
    cl_context m_Context;
    cl_device_id device;
    cl_command_queue queue;
    cl_program program;
    cl_kernel m_ResponseKernel;
    cl_kernel m_IPointsKernel;
    cl_kernel m_GetIPointsKernel;
    InitCL(m_Context, device, queue);
    CreateCLProgram(DATA_DIRECTORY"/Hessian.cl","ResponseLayerParallelV3",m_Context,device, program, m_ResponseKernel);
    CreateCLProgram(DATA_DIRECTORY"/Hessian.cl","IPointsParallelV2",m_Context,device, program, m_IPointsKernel);
    CreateCLProgram(DATA_DIRECTORY"/Hessian.cl","GetIPoints",m_Context,device, program, m_GetIPointsKernel);

	
    microsec initialisationTime = GetMicro();

	size_t m_ResponseKernelWorkSize;
	clGetKernelWorkGroupInfo(m_ResponseKernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &m_ResponseKernelWorkSize, NULL);
	size_t m_IPointsKernelWorkSize;
	clGetKernelWorkGroupInfo(m_IPointsKernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &m_IPointsKernelWorkSize, NULL);
	size_t GetIPointsKernelIC;
	clGetKernelWorkGroupInfo(m_GetIPointsKernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &GetIPointsKernelIC, NULL);


	
    float m_thresh = 0.002f;
    int m_octaves = 5;
    int m_initsample = 2;
	int m_DescriptorLength = 64;
	
	
	const int countresponselayers = 12;

    cl_image_format imgOutFormat;
	imgOutFormat.image_channel_order = CL_R;
	imgOutFormat.image_channel_data_type = CL_FLOAT;

	cl_mem integralImageOutput = clCreateImage2D(
        m_Context, 
		CL_MEM_READ_WRITE, 
        &imgOutFormat, 
		imageWidth, 
        imageHeight, 
        0, 
		0, 
        &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);


		size_t origin[] = {0,0,0};
		size_t region[] = {imageWidth,imageHeight,1};
		ErrorCode = clEnqueueWriteImage(queue, integralImageOutput, CL_TRUE, origin, region, 0, 0, integralImageData, 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);

       

	cl_mem m_HessianResponseBuffer = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, 
        sizeof(SResponseLayer) * countresponselayers, NULL, &ErrorCode);
	assert(ErrorCode == CL_SUCCESS);

    cl_mem m_HessianIpointsBuffer = clCreateBuffer(m_Context, CL_MEM_WRITE_ONLY, sizeof(SIpointSmall) * m_Height * m_Width, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);


    cl_mem m_HessianRBuffer = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, 
        sizeof(cl_float) *  m_Height * m_Width * countresponselayers, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);


    cl_mem m_HessianLBuffer = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, 
        sizeof(unsigned char) *  m_Height * m_Width * countresponselayers, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);


    // Responselayers brauchen ja nur einmal berechnet zu werden
    SResponseLayer* m_responselayers = new SResponseLayer[countresponselayers];
	int w = (m_Width / m_initsample);
	int h = (m_Height / m_initsample);
	int s = m_initsample;


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

	ErrorCode = clEnqueueWriteBuffer(queue, m_HessianResponseBuffer, CL_TRUE, 0, sizeof(SResponseLayer) * countresponselayers, m_responselayers, 0, NULL, NULL);
	assert(ErrorCode == CL_SUCCESS);

	SIpointSmall* m_TmpIpoints = new SIpointSmall[m_Width * m_Height];



//Hessian
	microsec startTime = GetMicro();
	int m_IPointCount = 0;

	for(int i = 0; i < traverseCount; i++)
	{
		int Layercount = countresponselayers;

		// buffer fŸr die Responses 
		ErrorCode = clSetKernelArg(m_ResponseKernel, 0, sizeof(cl_mem), &integralImageOutput);
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

		ErrorCode = clEnqueueNDRangeKernel(queue, m_ResponseKernel, 1, NULL, &m_ResponseKernelWorkSize, &m_ResponseKernelWorkSize, 0, NULL, NULL);
		assert(ErrorCode == CL_SUCCESS);
		//for ( int i = 0; i < countresponselayers; i++)
		//{

		//index -> index im Ziellayer (ein dimensionales Array)
		//ar und ac sind y und x Koordinate im Ziellayer


		// jetzt die Responses parallel berechnen


		//}

		//////// Parallelisieren der IPoints
		unsigned int *IPointCount = new unsigned int[m_IPointsKernelWorkSize];
		assert(IPointCount != NULL);
		for (int i = 0; i < m_IPointsKernelWorkSize; i++) {
			IPointCount[i] = 0;
		}

		
		cl_mem m_IpointsCount = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, 
			sizeof(cl_uint) * m_IPointsKernelWorkSize, NULL, &ErrorCode); 
		assert(ErrorCode == CL_SUCCESS);

		ErrorCode = clEnqueueWriteBuffer(queue, m_IpointsCount, CL_TRUE, 0, sizeof(cl_uint) * m_IPointsKernelWorkSize, IPointCount, 0, NULL, NULL);
		assert(ErrorCode == CL_SUCCESS);

		//???


		int filter_map[20] = {0,1,2,3,1,3,4,5,3,5,6,7,5,7,8,9,7,9,10,11};
		int Count = 0;

    
		memset(m_TmpIpoints, -1, m_Width * m_Height);
		ErrorCode = clEnqueueWriteBuffer(queue, m_HessianIpointsBuffer, CL_TRUE, 0, sizeof(SIpointSmall) * m_Height * m_Width, m_TmpIpoints, 0, NULL, NULL);
		assert(ErrorCode == CL_SUCCESS);
	
		ErrorCode = clSetKernelArg(m_IPointsKernel, 3, sizeof(cl_uint), &m_Width);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_IPointsKernel, 4, sizeof(cl_uint), &m_Height);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_IPointsKernel, 5, sizeof(cl_uint), &m_thresh);
		assert(ErrorCode == CL_SUCCESS);
		ErrorCode = clSetKernelArg(m_IPointsKernel, 6, sizeof(cl_mem), &m_IpointsCount);
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


				ErrorCode = clEnqueueNDRangeKernel(queue, m_IPointsKernel, 1, NULL, &m_IPointsKernelWorkSize, &m_IPointsKernelWorkSize, 0, NULL, NULL);
				assert(ErrorCode == CL_SUCCESS);
			}
		}

		// Wieviele Punkte wurden gefunden?
		ErrorCode = clEnqueueReadBuffer(queue, m_IpointsCount, CL_TRUE, 0, 
			sizeof(cl_uint) * m_IPointsKernelWorkSize, IPointCount, 0, NULL, NULL);
		assert(ErrorCode == CL_SUCCESS);

		for (int i = 0; i < m_IPointsKernelWorkSize; i++) {
			Count += IPointCount[i];
		}
	 
		// Buffer anlegen
		// nur wenn was gefunden
		if (Count > 0)
		{
			cl_mem m_GetIpoints = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, 
									 sizeof(SIpoint) * Count, NULL, &ErrorCode);
			assert(ErrorCode == CL_SUCCESS);
		
			// Parameter Ÿbergeben 
			ErrorCode = clSetKernelArg(m_GetIPointsKernel, 0, sizeof(cl_uint), &Count);
			assert(ErrorCode == CL_SUCCESS);
			ErrorCode = clSetKernelArg(m_GetIPointsKernel, 1, sizeof(cl_uint), &m_IPointsKernelWorkSize);
			assert(ErrorCode == CL_SUCCESS);
			ErrorCode = clSetKernelArg(m_GetIPointsKernel, 2, sizeof(cl_uint), &m_Width);
			assert(ErrorCode == CL_SUCCESS);
			ErrorCode = clSetKernelArg(m_GetIPointsKernel, 3, sizeof(cl_uint), &m_Height);
			assert(ErrorCode == CL_SUCCESS);
			ErrorCode = clSetKernelArg(m_GetIPointsKernel, 4, sizeof(cl_mem), &m_IpointsCount);
			assert(ErrorCode == CL_SUCCESS);
			ErrorCode = clSetKernelArg(m_GetIPointsKernel, 5, sizeof(cl_mem), &m_HessianIpointsBuffer);
			assert(ErrorCode == CL_SUCCESS);
			ErrorCode = clSetKernelArg(m_GetIPointsKernel, 6, sizeof(cl_mem), &m_GetIpoints);
			assert(ErrorCode == CL_SUCCESS);

			// Ipoints ins Array kopieren lassen
			size_t oneWorker = 1;
			ErrorCode = clEnqueueNDRangeKernel(queue, m_GetIPointsKernel, 1, 0, &oneWorker, &oneWorker, 0, NULL, NULL);
			assert(ErrorCode == CL_SUCCESS);

			SIpoint* m_Ipoints = new SIpoint[Count];

			ErrorCode = clEnqueueReadBuffer(queue, m_GetIpoints, CL_TRUE, 0, 
										sizeof(SIpoint) * Count, m_Ipoints, 0, NULL, NULL);  
			assert(ErrorCode == CL_SUCCESS);
		
			clFinish(queue);
	
			m_IPointCount = Count;
			clReleaseMemObject(m_GetIpoints);
		}
		clReleaseMemObject(m_IpointsCount);
		delete[] IPointCount;

	}
	
	microsec endTime = GetMicro();
    PrintResults("HessianV3", initialisationTime, startTime, endTime);

}
void BenchHessianRealImage()
{
	//ignore compiletime
    CSurfCL surf = CSurfCL();
	
	//buffer initialisation
    microsec initialisationTime = GetMicro();

    //start
    microsec startTime = GetMicro();
	
    //extratests um Geschwindigkeitsverlust über die Zeit zu messen (evtl. durch schlechte Speicherverwaltung)
    microsec firstEndTime; //Zeit des Abschluss des ersten Bildes
    microsec lastStartTime;//Zeit des Startes des letzten Bildes
	
	int Height = 0;
    int Width = 0;
    unsigned char* ImgData;
    surf.ReadBitmap(DATA_DIRECTORY"/cross.bmp", Width, Height, &ImgData);
	surf.DebugResizeBuffer(Width, Height);
	surf.CreateIntegralImage(ImgData);
    
    
	
    for(int i = 0; i < traverseCount; i++)
    {
        if(i == traverseCount-1)
            lastStartTime = GetMicro();
        
		surf.DebugScanImage(Width, Height, ImgData, 0.0002f, 5, 2, false);
		
        if(i == 0)
            firstEndTime = GetMicro();
    }
	
    microsec endTime = GetMicro();
	
	delete[] ImgData;
    PrintResults("Hessian (Kreuz)", initialisationTime, startTime, endTime);
    
    std::cout<<std::endl;
    std::cout<<"\t"<<"first Image duration:      "<<firstEndTime - startTime<<" microsec"<<std::endl;
    std::cout<<"\t"<<"last Image duration:       "<<endTime - lastStartTime<<" microsec"<<std::endl;
}

void BenchCarstensImage()
{
    //ignore compiletime
    CSurfCL surf = CSurfCL();


    //buffer initialisation
    microsec initialisationTime = GetMicro();
	
    //start
    microsec startTime = GetMicro();
	
    //extratests um Geschwindigkeitsverlust über die Zeit zu messen (evtl. durch schlechte Speicherverwaltung)
    microsec firstEndTime; //Zeit des Abschluss des ersten Bildes
    microsec lastStartTime;//Zeit des Startes des letzten Bildes
	
    for(int i = 0; i < traverseCount; i++)
    {
        if(i == traverseCount-1)
            lastStartTime = GetMicro();
        surf.DebugScanFromBitmap(DATA_DIRECTORY"/pic_carsten.bmp", 0.0002f, 5, 2, false);
        int count = surf.GetIPointsCount();
		//surf.CalculateSurfDescriptor();
		
        if(i == 0)
            firstEndTime = GetMicro();
    }
	
    microsec endTime = GetMicro();
	
    PrintResults("RealImage (Carstens Bild)", initialisationTime, startTime, endTime);
    
    std::cout<<std::endl;
    std::cout<<"\t"<<"first Image duration:      "<<firstEndTime - startTime<<" microsec"<<std::endl;
    std::cout<<"\t"<<"last Image duration:       "<<endTime - lastStartTime<<" microsec"<<std::endl;
}

void BenchDescriptorsCarstensImage()
{
    //ignore compiletime
    CSurfCL surf = CSurfCL();

    //buffer initialisation
    microsec initialisationTime = GetMicro();
	
    //start
    microsec startTime = GetMicro();
	
    //extratests um Geschwindigkeitsverlust über die Zeit zu messen (evtl. durch schlechte Speicherverwaltung)
    microsec firstEndTime; //Zeit des Abschluss des ersten Bildes
    microsec lastStartTime;//Zeit des Startes des letzten Bildes
	
	surf.DebugScanFromBitmap(DATA_DIRECTORY"/pic_carsten.bmp", 0.0002f, 5, 2, false);
	int count = surf.GetIPointsCount();
    for(int i = 0; i < traverseCount; i++)
    {
        if(i == traverseCount-1)
            lastStartTime = GetMicro();
        
		surf.CalculateSurfDescriptor();
		
        if(i == 0)
            firstEndTime = GetMicro();
    }
	
    microsec endTime = GetMicro();
	
    PrintResults("BenchDescriptorsCarstensImage (Carstens Bild)", initialisationTime, startTime, endTime);
    
    std::cout<<std::endl;
    std::cout<<"\t"<<"first Image duration:      "<<firstEndTime - startTime<<" microsec"<<std::endl;
    std::cout<<"\t"<<"last Image duration:       "<<endTime - lastStartTime<<" microsec"<<std::endl;
}

void BenchRealImage()
{
    //ignore compiletime
    CSurfCL surf = CSurfCL();


    //buffer initialisation
    microsec initialisationTime = GetMicro();
	
    //start
    microsec startTime = GetMicro();
	
    //extratests um Geschwindigkeitsverlust über die Zeit zu messen (evtl. durch schlechte Speicherverwaltung)
    microsec firstEndTime; //Zeit des Abschluss des ersten Bildes
    microsec lastStartTime;//Zeit des Startes des letzten Bildes
	
    for(int i = 0; i < traverseCount; i++)
    {
        if(i == traverseCount-1)
            lastStartTime = GetMicro();
        surf.DebugScanFromBitmap(DATA_DIRECTORY"/cross.bmp", 0.0002f, 5, 2, false);
        surf.CalculateSurfDescriptor();
        if(i == 0)
            firstEndTime = GetMicro();
    }
	
    microsec endTime = GetMicro();
	
    PrintResults("RealImage (Kreuz)", initialisationTime, startTime, endTime);
    
    std::cout<<std::endl;
    std::cout<<"\t"<<"first Image duration:      "<<firstEndTime - startTime<<" microsec"<<std::endl;
    std::cout<<"\t"<<"last Image duration:       "<<endTime - lastStartTime<<" microsec"<<std::endl;
}

void BenchCurrentVersion()
{
    //ignore compiletime
    CSurfCL surf = CSurfCL();

    

    //buffer initialisation
    microsec initialisationTime = GetMicro();
    surf.DebugResizeBuffer(imageWidth, imageHeight);

    //start
    microsec startTime = GetMicro();

    //extratests um Geschwindigkeitsverlust über die Zeit zu messen (evtl. durch schlechte Speicherverwaltung)
    microsec firstEndTime; //Zeit des Abschluss des ersten Bildes
    microsec lastStartTime;//Zeit des Startes des letzten Bildes

    for(int i = 0; i < traverseCount; i++)
    {
        if(i == traverseCount-1)
            lastStartTime = GetMicro();
        surf.ScanImage24Bit(imageWidth, imageHeight, imageData, 0.0002f, 5, 2, false);
		surf.CalculateSurfDescriptor();
        if(i == 0)
            firstEndTime = GetMicro();
    }

    microsec endTime = GetMicro();

    PrintResults("CurrentVersion", initialisationTime, startTime, endTime);
    
    std::cout<<std::endl;
    std::cout<<"\t"<<"first Image duration:      "<<firstEndTime - startTime<<" microsec"<<std::endl;
    std::cout<<"\t"<<"last Image duration:       "<<endTime - lastStartTime<<" microsec"<<std::endl;
}

void BenchIntegralImageCPU()
{
    const float cR = 0.2989f;
    const float cG = 0.5870f;
    const float cB = 0.1140f;
    microsec initialisationTime = GetMicro();
    float* result = new float[imageWidth * imageHeight * 3];
    microsec startTime = GetMicro();

    for(int i = 0; i < traverseCount; i++)
    {
        float rowsum = 0;
        for (int x = 0; x < imageWidth; x++)
        {
            float r = imageData[x*3 + 2];
            float g = imageData[x*3 + 1];
            float b = imageData[x*3];
            rowsum += (cR * r + cG * g + cB * b) / 255.0f;
            result[x] = rowsum;
        }
        for (int y = 1; y < imageHeight; y++)
        {
            rowsum = 0;
            for (int x = 0; x < imageWidth; x++)
            {
                float r = imageData[(y * imageWidth + x) * 3 + 2];
                float g = imageData[(y * imageWidth + x) * 3 + 1];
                float b = imageData[(y * imageWidth + x) * 3];
            
                rowsum += (cR * r + cG * g + cB * b) / 255.0f;
            
                // integral image is rowsum + value above
                result[y * imageWidth + x] = rowsum + result[(y - 1) * imageWidth + x];
            }
        }
    }
    std::cout << "lastVal: " << result[imageHeight*imageWidth-1] << std::endl;
    microsec endTime = GetMicro();

    PrintResults("IntegralImageCPU", initialisationTime, startTime, endTime);
	delete[] result;
}


void BenchIntegralImageCurrentVersion()
{
    //ignore compiletime
    CSurfCL surf = CSurfCL();


    //buffer initialisation
    microsec initialisationTime = GetMicro();
    surf.DebugResizeBuffer(imageWidth, imageHeight);
    float* result = new float[imageHeight*imageWidth];
    //start
    microsec startTime = GetMicro();

    //extratests um Geschwindigkeitsverlust über die Zeit zu messen (evtl. durch schlechte Speicherverwaltung)
    microsec firstEndTime; //Zeit des Abschluss des ersten Bildes
    microsec lastStartTime;//Zeit des Startes des letzten Bildes
    
    for(int i = 0; i < traverseCount; i++)
    {
        if(i == traverseCount-1)
            lastStartTime = GetMicro();

        surf.DebugCreateIntegralImage(imageData);
         //das Lesen stellt sicher, dass der Kernel ausgeführt wurde
        surf.DebugGetIntegralImage(result);
        
        if(i == 0)
            firstEndTime = GetMicro();
    }

    microsec endTime = GetMicro();
    delete[] result;
    PrintResults("IntegralImageCurrentVersion", initialisationTime, startTime, endTime);
    
    std::cout<<std::endl;
    std::cout<<"\t"<<"first Image duration:      "<<firstEndTime - startTime<<" microsec"<<std::endl;
    std::cout<<"\t"<<"last Image duration:       "<<endTime - lastStartTime<<" microsec"<<std::endl;
}

void BenchIntegralImageParallel()
{
    cl_context context;
    cl_device_id device;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    InitCL(context, device, queue);
    CreateCLProgram(DATA_DIRECTORY"/IntegralImage.cl","CreateIntegralImageParallel",context,device, program, kernel);

	size_t benchWorkItemCount;
	clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &benchWorkItemCount, NULL);
	

    microsec initialisationTime = GetMicro();
    cl_int ErrorCode;

    size_t size = imageHeight * imageWidth * 3;

    //IntegralImage
    cl_mem integralImageInput = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar) * imageHeight * imageWidth * 3, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

	cl_mem integralImageOutput = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float) * imageHeight * imageWidth, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

    microsec startTime = GetMicro();

    for( int i = 0; i < traverseCount; i++)
    {

        ErrorCode = clEnqueueWriteBuffer(queue, integralImageInput, CL_TRUE, 0, sizeof(cl_uchar) * size, &imageData[0], 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);


        ErrorCode = clSetKernelArg(kernel, 0, sizeof(cl_uint), &imageHeight);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel, 1, sizeof(cl_uint), &imageWidth);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel, 2, sizeof(cl_mem), &integralImageInput);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel, 3, sizeof(cl_mem), &integralImageOutput);
        assert(ErrorCode == CL_SUCCESS);

    
        ErrorCode = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &benchWorkItemCount, &benchWorkItemCount, 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);
        
		clFinish(queue);
    }
    microsec endTime = GetMicro();

    PrintResults("IntegralImageParallel", initialisationTime, startTime, endTime);

    clReleaseMemObject(integralImageInput);
    clReleaseMemObject(integralImageOutput);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}
void BenchIntegralImageParallelV2()
{
    cl_context context;
    cl_device_id device;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    InitCL(context, device, queue);
    CreateCLProgram(DATA_DIRECTORY"/IntegralImage.cl","CreateIntegralImageParallelV2",context,device, program, kernel);
    
	
	
    
    microsec initialisationTime = GetMicro();
    cl_int ErrorCode;

    size_t size = imageHeight * imageWidth * 3;

    //IntegralImage
    cl_mem integralImageInput = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar) * imageHeight * imageWidth * 3, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

	cl_mem integralImageOutput = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float) * imageHeight * imageWidth, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

    microsec startTime = GetMicro();

	size_t benchWorkItemCount;
	clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &benchWorkItemCount, NULL);

    for( int i = 0; i < traverseCount; i++)
    {
		int s = sizeof(cl_uchar);
        //write in buffer
        //Achtung: Hier war ein Fehler CL_FALSE schreibt den Buffer parallel, während das Programm weiter ausgeführt wird
        //Im Releasemode waren daher die Ergebnisse falsch, weil er das OpenCL Programm ausgeführt hat bevor der Buffer voll war
        //ErrorCode = clEnqueueWriteBuffer(*m_CommandQueue, m_InputBuffer, CL_FALSE, 0, sizeof(cl_uchar) * size, &imageData[0], 0, NULL, NULL);
        ErrorCode = clEnqueueWriteBuffer(queue, integralImageInput, CL_TRUE, 0, sizeof(cl_uchar) * size, &imageData[0], 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);

        //set the buffer as argument
    

        ErrorCode = clSetKernelArg(kernel, 0, sizeof(cl_uint), &imageHeight);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel, 1, sizeof(cl_uint), &imageWidth);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel, 2, sizeof(cl_mem), &integralImageInput);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel, 3, sizeof(cl_mem), &integralImageOutput);
        assert(ErrorCode == CL_SUCCESS);
        int pixelSize = 3;
        ErrorCode = clSetKernelArg(kernel, 4, sizeof(int), &pixelSize);
        assert(ErrorCode == CL_SUCCESS);


		
        ErrorCode = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &benchWorkItemCount, &benchWorkItemCount, 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);
        clFinish(queue);
    }
    microsec endTime = GetMicro();
	
	float* integralImage = new float[imageHeight*imageWidth];
	clEnqueueReadBuffer(queue, integralImageOutput, CL_TRUE, 0, sizeof(cl_float) * imageHeight * imageWidth, integralImage,0,0,0);
	std::cout << "lastVal: " << integralImage[imageHeight*imageWidth-1] << std::endl;
	delete[] integralImage;
    PrintResults("IntegralImageParallelV2", initialisationTime, startTime, endTime);

    clReleaseMemObject(integralImageInput);
    clReleaseMemObject(integralImageOutput);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}

void BenchIntegralImageParallelV3_24Bit()
{
    cl_context context;
    cl_device_id device;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    InitCL(context, device, queue);
    
     
    cl_bool imgsup;
    clGetDeviceInfo(device, CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &imgsup, 0);;
	if(!imgsup)
    {
        clReleaseContext(context);
        clReleaseCommandQueue(queue);
        return;
    }

    CreateCLProgram(DATA_DIRECTORY"/IntegralImage.cl","CreateIntegralImageParallelV3_24Bit",context, device, program, kernel);
    
	
    
    size_t benchWorkItemCount;
	clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &benchWorkItemCount, NULL);

    microsec initialisationTime = GetMicro();
    cl_int ErrorCode;


    //IntegralImage
    cl_image_format imgFormat;
	imgFormat.image_channel_order = CL_R;
	imgFormat.image_channel_data_type = CL_UNSIGNED_INT8;

    cl_mem integralImageInput = clCreateImage2D(
        context, 
		CL_MEM_READ_ONLY, 
        &imgFormat, 
		imageWidth*3, 
        imageHeight, 
        0, 
		imageData, 
        &ErrorCode);

   
    //cl_mem integralImageInput = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar) * imageHeight * imageWidth * 3, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);


	cl_mem integralImageOutput = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float) * imageWidth * imageHeight, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

    microsec startTime = GetMicro();

    for( int i = 0; i < traverseCount; i++)
    {
        size_t origin[] = {0,0,0};
		size_t region[] = {imageWidth*3,imageHeight,1};
        ErrorCode = clEnqueueWriteImage(queue, integralImageInput, CL_TRUE, origin, region, 0, 0, imageData, 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);

        //set the buffer as argument

        ErrorCode = clSetKernelArg(kernel, 0, sizeof(cl_mem), &integralImageInput);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel, 1, sizeof(cl_mem), &integralImageOutput);
        assert(ErrorCode == CL_SUCCESS);

    
		ErrorCode = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &benchWorkItemCount, &benchWorkItemCount, 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);
        clFinish(queue);
    }
    microsec endTime = GetMicro();
	float* integralImage = new float[imageWidth * imageHeight];
	clEnqueueReadBuffer(queue, integralImageOutput, CL_TRUE, 0, sizeof(cl_float) * imageWidth * imageHeight, integralImage, 0, 0, 0);
	//for(int i = 0; i < imageHeight; i++)
	//{
	//	for(int j = 0; j < imageWidth; j++)
	//	{
	//		std::cout<<integralImage[i*imageWidth+j]<<" ";
	//	}
	//	std::cout<<std::endl;
	//}
	std::cout << "lastVal: " << integralImage[imageWidth*imageHeight-1] << std::endl;
	delete[] integralImage;
    PrintResults("IntegralImageParallelV3_24Bit", initialisationTime, startTime, endTime);

    clReleaseMemObject(integralImageInput);	

    clReleaseMemObject(integralImageOutput);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    /*delete[] imageData32;*/
}

void BenchIntegralImageParallelV3_32Bit()
{
    
    unsigned char* imageData32 = new unsigned char[imageHeight*imageWidth*4];
    for(int i=0; i < imageHeight*imageWidth; i++)
    {
        imageData32[i*4] =   imageData[i*3 + 2]; //x -> r
        imageData32[i*4+1] = imageData[i*3 + 1]; //y -> b
        imageData32[i*4+2] = imageData[i*3 + 0]; //z -> g
        imageData32[i*4+3] = 0;//; //w
    }
    cl_context context;
    cl_device_id device;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    InitCL(context, device, queue);
     
    cl_bool imgsup;
    clGetDeviceInfo(device, CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &imgsup, 0);
	if(!imgsup)
    {
        clReleaseContext(context);
        clReleaseCommandQueue(queue);
        return;
    }
    CreateCLProgram(DATA_DIRECTORY"/IntegralImage.cl","CreateIntegralImageParallelV3_32Bit",context,device, program, kernel);

    
    size_t benchWorkItemCount;
	clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &benchWorkItemCount, NULL);

    microsec initialisationTime = GetMicro();
    cl_int ErrorCode;

    size_t size = imageHeight * imageWidth * 3;

    //IntegralImage
    cl_image_format imgFormat;
	imgFormat.image_channel_order = CL_RGBA;
    imgFormat.image_channel_data_type = CL_UNORM_INT8;

    cl_mem integralImageInput = clCreateImage2D(
        context, 
		CL_MEM_READ_ONLY, 
        &imgFormat, 
        imageWidth, 
        imageHeight, 
        NULL, 
        NULL, 
        &ErrorCode);

   
    //cl_mem integralImageInput = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar) * imageHeight * imageWidth * 3, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

    cl_mem integralImageOutput = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float) * imageHeight * imageWidth, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

    microsec startTime = GetMicro();

    for( int i = 0; i < traverseCount; i++)
    {
        size_t origin[] = {0,0,0};
        size_t region[] = {imageWidth,imageHeight,1};
        ErrorCode = clEnqueueWriteImage(queue, integralImageInput, CL_TRUE, origin, region, imageWidth*4, 0, &imageData32[0], 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);

        //set the buffer as argument

        ErrorCode = clSetKernelArg(kernel, 0, sizeof(cl_mem), &integralImageInput);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel, 1, sizeof(cl_mem), &integralImageOutput);
        assert(ErrorCode == CL_SUCCESS);

    
		ErrorCode = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &benchWorkItemCount, &benchWorkItemCount, 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);
        clFinish(queue);
    }
    microsec endTime = GetMicro();
	float* integralImage = new float[imageHeight*imageWidth];
	clEnqueueReadBuffer(queue, integralImageOutput, CL_TRUE, 0, sizeof(cl_float) * imageHeight * imageWidth, integralImage,0,0,0);
	std::cout << "lastVal: " << integralImage[imageHeight*imageWidth-1] << std::endl;
	delete[] integralImage;
    PrintResults("IntegralImageParallelV3_32Bit", initialisationTime, startTime, endTime);

    clReleaseMemObject(integralImageInput);	
    clReleaseMemObject(integralImageOutput);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    delete[] imageData32;
}

void BenchIntegralImageParallelV3_24Bit_()
{
    cl_context context;
    cl_device_id device;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    InitCL(context, device, queue);
    CreateCLProgram(DATA_DIRECTORY"/IntegralImage.cl","CreateIntegralImageParallelV3_24Bit_",context, device, program, kernel);
    
	
    
    size_t benchWorkItemCount;
	clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &benchWorkItemCount, NULL);

    microsec initialisationTime = GetMicro();
    cl_int ErrorCode;


    //IntegralImage
    cl_image_format imgFormat;
	imgFormat.image_channel_order = CL_R;
	imgFormat.image_channel_data_type = CL_UNSIGNED_INT8;

    cl_mem integralImageInput = clCreateImage2D(
        context, 
		CL_MEM_READ_ONLY, 
        &imgFormat, 
		imageWidth*3, 
        imageHeight, 
        0, 
		imageData, 
        &ErrorCode);

   
    //cl_mem integralImageInput = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar) * imageHeight * imageWidth * 3, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);


	cl_image_format imgOutFormat;
	imgOutFormat.image_channel_order = CL_R;
	imgOutFormat.image_channel_data_type = CL_FLOAT;

    cl_mem integralImageOutput = clCreateImage2D(
        context, 
		CL_MEM_READ_WRITE, 
        &imgOutFormat, 
		imageWidth, 
        imageHeight, 
        0, 
		0, 
        &ErrorCode);
	//cl_mem integralImageOutput = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float) * imageWidth * imageHeight, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

	cl_mem integralImageBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float) * imageWidth * imageHeight, NULL, &ErrorCode);
	assert(ErrorCode == CL_SUCCESS);

    microsec startTime = GetMicro();

    for( int i = 0; i < traverseCount; i++)
    {
        size_t origin[] = {0,0,0};
		size_t region[] = {imageWidth*3,imageHeight,1};
        ErrorCode = clEnqueueWriteImage(queue, integralImageInput, CL_TRUE, origin, region, 0, 0, imageData, 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);

        //set the buffer as argument

        ErrorCode = clSetKernelArg(kernel, 0, sizeof(cl_mem), &integralImageInput);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel, 1, sizeof(cl_mem), &integralImageOutput);
        assert(ErrorCode == CL_SUCCESS);

		ErrorCode = clSetKernelArg(kernel, 2, sizeof(cl_mem), &integralImageBuffer);
        assert(ErrorCode == CL_SUCCESS);

    
		ErrorCode = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &benchWorkItemCount, &benchWorkItemCount, 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);
        clFinish(queue);
    }
    microsec endTime = GetMicro();
	float* integralImage = new float[imageWidth * imageHeight];
	//clEnqueueReadBuffer(queue, integralImageOutput, CL_TRUE, 0, sizeof(cl_float) * imageWidth * imageHeight, integralImage, 0, 0, 0);
	size_t origin[] = {0,0,0};
	size_t region[] = {imageWidth,imageHeight,1};
	clEnqueueReadImage(queue, integralImageOutput, CL_TRUE, origin, region, 0, 0, integralImage, 0, NULL, NULL);
	//for(int i = 0; i < imageHeight; i++)
	//{
	//	for(int j = 0; j < imageWidth; j++)
	//	{
	//		std::cout<<integralImage[i*imageWidth+j]<<" ";
	//	}
	//	std::cout<<std::endl;
	//}
	std::cout << "lastVal: " << integralImage[imageWidth*imageHeight-1] << std::endl;
	delete[] integralImage;
    PrintResults("IntegralImageParallelV3_24_", initialisationTime, startTime, endTime);

    clReleaseMemObject(integralImageInput);	

    clReleaseMemObject(integralImageOutput);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    /*delete[] imageData32;*/
}

void BenchIntegralImageParallelV3_24BitTwoParts()
{
    cl_context context;
    cl_device_id device;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    cl_kernel kernel2;
    InitCL(context, device, queue);
    CreateCLProgram(DATA_DIRECTORY"/IntegralImage.cl","CreateIntegralImageParallelV3_24BitTwoParts1",context, device, program, kernel);
    CreateCLProgram(DATA_DIRECTORY"/IntegralImage.cl","CreateIntegralImageParallelV3_24BitTwoParts2",context, device, program, kernel2);
    
	
    
    size_t benchWorkItemCount1;
	clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &benchWorkItemCount1, NULL);
	
	size_t benchWorkItemCount2;
	clGetKernelWorkGroupInfo(kernel2, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &benchWorkItemCount2, NULL);

    microsec initialisationTime = GetMicro();
    cl_int ErrorCode;


    //IntegralImage
    cl_image_format imgFormat;
	imgFormat.image_channel_order = CL_R;
	imgFormat.image_channel_data_type = CL_UNSIGNED_INT8;

    cl_mem integralImageInput = clCreateImage2D(
        context, 
		CL_MEM_READ_WRITE, 
        &imgFormat, 
		imageWidth*3, 
        imageHeight, 
        0, 
		imageData, 
        &ErrorCode);

   
    //cl_mem integralImageInput = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar) * imageHeight * imageWidth * 3, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);


	cl_image_format imgOutFormat;
	imgOutFormat.image_channel_order = CL_R;
	imgOutFormat.image_channel_data_type = CL_FLOAT;

    cl_mem integralImageBuffer = clCreateImage2D(
        context, 
		CL_MEM_READ_WRITE, 
        &imgOutFormat, 
		imageWidth, 
        imageHeight, 
        0, 
		0, 
        &ErrorCode);


    cl_mem integralImageOutput = clCreateImage2D(
        context, 
		CL_MEM_READ_WRITE, 
        &imgOutFormat, 
		imageWidth, 
        imageHeight, 
        0, 
		0, 
        &ErrorCode);
	//cl_mem integralImageOutput = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float) * imageWidth * imageHeight, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

	//cl_mem integralImageBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float) * imageWidth * imageHeight, NULL, &ErrorCode);
	assert(ErrorCode == CL_SUCCESS);

    microsec startTime = GetMicro();

    for( int i = 0; i < traverseCount; i++)
    {
        size_t origin[] = {0,0,0};
		size_t region[] = {imageWidth*3,imageHeight,1};
        ErrorCode = clEnqueueWriteImage(queue, integralImageInput, CL_TRUE, origin, region, 0, 0, imageData, 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);

        //set the buffer as argument

        ErrorCode = clSetKernelArg(kernel, 0, sizeof(cl_mem), &integralImageInput);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel, 1, sizeof(cl_mem), &integralImageBuffer);
        assert(ErrorCode == CL_SUCCESS);

    
		ErrorCode = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &benchWorkItemCount1, &benchWorkItemCount1, 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);



        ErrorCode = clSetKernelArg(kernel2, 0, sizeof(cl_mem), &integralImageBuffer);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel2, 1, sizeof(cl_mem), &integralImageOutput);
        assert(ErrorCode == CL_SUCCESS);
    
		ErrorCode = clEnqueueNDRangeKernel(queue, kernel2, 1, NULL, &benchWorkItemCount2, &benchWorkItemCount2, 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);
        clFinish(queue);
    }
    microsec endTime = GetMicro();
	float* integralImage = new float[imageWidth * imageHeight];
	//clEnqueueReadBuffer(queue, integralImageOutput, CL_TRUE, 0, sizeof(cl_float) * imageWidth * imageHeight, integralImage, 0, 0, 0);
	size_t origin[] = {0,0,0};
	size_t region[] = {imageWidth,imageHeight,1};
	clEnqueueReadImage(queue, integralImageOutput, CL_TRUE, origin, region, 0, 0, integralImage, 0, NULL, NULL);
	//for(int i = 0; i < imageHeight; i++)
	//{
	//	for(int j = 0; j < imageWidth; j++)
	//	{
	//		std::cout<<integralImage[i*imageWidth+j]<<" ";
	//	}
	//	std::cout<<std::endl;
	//}
	std::cout << "lastVal: " << integralImage[imageWidth*imageHeight-1] << std::endl;
	delete[] integralImage;
    PrintResults("IntegralImageParallelV3_24TwoParts", initialisationTime, startTime, endTime);

    clReleaseMemObject(integralImageInput);	

    clReleaseMemObject(integralImageOutput);
    clReleaseKernel(kernel);
    clReleaseKernel(kernel2);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    /*delete[] imageData32;*/
}

void BenchIntegralImageSequencial()
{
    cl_context context;
    cl_device_id device;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    InitCL(context, device, queue);
    CreateCLProgram(DATA_DIRECTORY"/IntegralImage.cl","CreateIntegralImageSequencial",context,device, program, kernel);

    cl_int ErrorCode;
    size_t size = imageHeight * imageWidth * 3;

    microsec initialisationTime = GetMicro();

    //IntegralImage
    cl_mem integralImageInput = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar) * imageHeight * imageWidth * 3, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

    cl_mem integralImageOutput = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_float) * imageHeight * imageWidth, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

    
    microsec startTime = GetMicro();

    for(int i = 0; i < traverseCount; i++)
    {
        ErrorCode = clEnqueueWriteBuffer(queue, integralImageInput, CL_TRUE, 0, sizeof(cl_uchar) * size, &imageData[0], 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);

        //set the buffer as argument
    

        ErrorCode = clSetKernelArg(kernel, 0, sizeof(cl_uint), &imageHeight);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel, 1, sizeof(cl_uint), &imageWidth);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel, 2, sizeof(cl_mem), &integralImageInput);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel, 3, sizeof(cl_mem), &integralImageOutput);
        assert(ErrorCode == CL_SUCCESS);

        size_t workerCount = 1;
    
        ErrorCode = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &workerCount, &workerCount, 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);
        clFinish(queue);
    }
    microsec endTime = GetMicro();

    PrintResults("IntegralImageSequencial", initialisationTime, startTime, endTime);

    clReleaseMemObject(integralImageInput);
    clReleaseMemObject(integralImageOutput);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}


void BenchIntegralImageParallelTwoPartsH()
{
    cl_context context;
    cl_device_id device;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    cl_program program2;
    cl_kernel kernel2;
    InitCL(context, device, queue);
    CreateCLProgram(DATA_DIRECTORY"/IntegralImage.cl","CreateIntegralImageParallelTwoParts1H",context,device, program, kernel);
    CreateCLProgram(DATA_DIRECTORY"/IntegralImage.cl","CreateIntegralImageParallelTwoParts2H",context,device, program2, kernel2);
    
    
	size_t benchWorkItemCount;
	clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &benchWorkItemCount, NULL);

    microsec initialisationTime = GetMicro();
    cl_int ErrorCode;

    size_t size = imageHeight * imageWidth * 3;

    //IntegralImage
    cl_mem integralImageInput = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar) * imageHeight * imageWidth * 3, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

    cl_mem integralImageOutput = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_float) * imageHeight * imageWidth, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

    microsec startTime = GetMicro();

    for( int i = 0; i < traverseCount; i++)
    {
        //write in buffer
        //Achtung: Hier war ein Fehler CL_FALSE schreibt den Buffer parallel, während das Programm weiter ausgeführt wird
        //Im Releasemode waren daher die Ergebnisse falsch, weil er das OpenCL Programm ausgeführt hat bevor der Buffer voll war
        //ErrorCode = clEnqueueWriteBuffer(*m_CommandQueue, m_InputBuffer, CL_FALSE, 0, sizeof(cl_uchar) * size, &imageData[0], 0, NULL, NULL);
        ErrorCode = clEnqueueWriteBuffer(queue, integralImageInput, CL_TRUE, 0, sizeof(cl_uchar) * size, &imageData[0], 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);

        //set the buffer as argument
    

        ErrorCode = clSetKernelArg(kernel, 0, sizeof(cl_uint), &imageHeight);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel, 1, sizeof(cl_uint), &imageWidth);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel, 2, sizeof(cl_mem), &integralImageInput);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel, 3, sizeof(cl_mem), &integralImageOutput);
        assert(ErrorCode == CL_SUCCESS);

    
        ErrorCode = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &benchWorkItemCount, &benchWorkItemCount, 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel2, 0, sizeof(cl_uint), &imageHeight);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel2, 1, sizeof(cl_uint), &imageWidth);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel2, 2, sizeof(cl_mem), &integralImageInput);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel2, 3, sizeof(cl_mem), &integralImageOutput);
        assert(ErrorCode == CL_SUCCESS);

		ErrorCode = clEnqueueNDRangeKernel(queue, kernel2, 1, NULL, &benchWorkItemCount, &benchWorkItemCount, 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);
        clFinish(queue);
    }
    microsec endTime = GetMicro();

    PrintResults("IntegralImageParallelTwoPartsH", initialisationTime, startTime, endTime);

    clReleaseMemObject(integralImageInput);
    clReleaseMemObject(integralImageOutput);
    clReleaseKernel(kernel);
    clReleaseKernel(kernel2);
    clReleaseProgram(program);
    clReleaseProgram(program2);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}

void BenchIntegralImageParallelTwoPartsV()
{
    cl_context context;
    cl_device_id device;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    cl_program program2;
    cl_kernel kernel2;
    InitCL(context, device, queue);
    CreateCLProgram(DATA_DIRECTORY"/IntegralImage.cl","CreateIntegralImageParallelTwoParts1V",context,device, program, kernel);
    CreateCLProgram(DATA_DIRECTORY"/IntegralImage.cl","CreateIntegralImageParallelTwoParts2V",context,device, program2, kernel2);
    
    
	size_t benchWorkItemCount;
	clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &benchWorkItemCount, NULL);

    microsec initialisationTime = GetMicro();
    cl_int ErrorCode;

    size_t size = imageHeight * imageWidth * 3;

    //IntegralImage
    cl_mem integralImageInput = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar) * imageHeight * imageWidth * 3, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

    cl_mem integralImageOutput = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_float) * imageHeight * imageWidth, NULL, &ErrorCode);
    assert(ErrorCode == CL_SUCCESS);

    microsec startTime = GetMicro();

    for( int i = 0; i < traverseCount; i++)
    {
        //write in buffer
        //Achtung: Hier war ein Fehler CL_FALSE schreibt den Buffer parallel, während das Programm weiter ausgeführt wird
        //Im Releasemode waren daher die Ergebnisse falsch, weil er das OpenCL Programm ausgeführt hat bevor der Buffer voll war
        //ErrorCode = clEnqueueWriteBuffer(*m_CommandQueue, m_InputBuffer, CL_FALSE, 0, sizeof(cl_uchar) * size, &imageData[0], 0, NULL, NULL);
        ErrorCode = clEnqueueWriteBuffer(queue, integralImageInput, CL_TRUE, 0, sizeof(cl_uchar) * size, &imageData[0], 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);

        //set the buffer as argument
    

        ErrorCode = clSetKernelArg(kernel, 0, sizeof(cl_uint), &imageHeight);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel, 1, sizeof(cl_uint), &imageWidth);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel, 2, sizeof(cl_mem), &integralImageInput);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel, 3, sizeof(cl_mem), &integralImageOutput);
        assert(ErrorCode == CL_SUCCESS);


    
        ErrorCode = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &benchWorkItemCount, &benchWorkItemCount, 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel2, 0, sizeof(cl_uint), &imageHeight);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel2, 1, sizeof(cl_uint), &imageWidth);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel2, 2, sizeof(cl_mem), &integralImageInput);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clSetKernelArg(kernel2, 3, sizeof(cl_mem), &integralImageOutput);
        assert(ErrorCode == CL_SUCCESS);

        ErrorCode = clEnqueueNDRangeKernel(queue, kernel2, 1, NULL, &benchWorkItemCount, &benchWorkItemCount, 0, NULL, NULL);
        assert(ErrorCode == CL_SUCCESS);
        clFinish(queue);
    }
    microsec endTime = GetMicro();

    PrintResults("IntegralImageParallelTwoPartsV", initialisationTime, startTime, endTime);

    clReleaseMemObject(integralImageInput);
    clReleaseMemObject(integralImageOutput);
    clReleaseKernel(kernel);
    clReleaseKernel(kernel2);
    clReleaseProgram(program);
    clReleaseProgram(program2);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}
void CL_CALLBACK ErrorHandler(const char * _Msg, const void * _Info, size_t cb, void * _UserData)
{
    std::cout << _Msg;
}
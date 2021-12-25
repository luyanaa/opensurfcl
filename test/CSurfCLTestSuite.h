#include <cxxtest/TestSuite.h>
#include <CSurfCL.h>
#include <iostream>
class CSurfCLTestSuite : public CxxTest::TestSuite
{
public:
    
    void testIntegralImageGreyscale( void )
    {
        std::cout<<std::endl<<"Start:"<<"testIntegralImageGreyscale"<<std::endl;
        const int width = 1000;
        const int height = 1000;
        
        const float cR = 0.2989f;
        const float cG = 0.5870f;
        const float cB = 0.1140f;

        const size_t size = width*height;
        unsigned char* imgdata = new unsigned char[size];
        for(int i = 0; i < size; i++)
        {
            imgdata[i] = 0;
        }

        for(int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
                int index = (y*width+x);
                float b = (unsigned char)(x * (255.0f/width));
                float g = (unsigned char)(y * (255.0f/height));
                float r =  (unsigned char)((x+y) * (255.0f/(width+height)));
                imgdata[index] = (r * cR + g * cG +  b * cB);
            }
        }
        CSurfCL surf = CSurfCL();
        surf.ScanImageGreyscale(width, height, imgdata,  0.0002f, 5, 2, false);
        float* data = new float[surf.GetLastHeight() * surf.GetLastWidth()];
        surf.DebugGetIntegralImage(data);

        float res = data[height * width - 1];
        delete[] data;
        delete[] imgdata;

        std::cout<<"calculated result: \t" << res << std::endl;

        //wegen der Rundung zu byte kommt hier ein anderer Wert raus.
        TS_ASSERT(res == 495520.47f  //ati
			|| res == 495520.41f //nvidia
			); 
        std::cout<<"End :"<<"testIntegralImageGreyscale"<<std::endl;
    }
    void testIntegralImage24Bit( void )
    {
        std::cout<<std::endl<<"Start:"<<"testIntegralImage24Bit"<<std::endl;
        const int width = 1000;
        const int height = 1000;

        const size_t size = width*height*3;
        unsigned char* imgdata = new unsigned char[size];
        for(int i = 0; i < size; i++)
        {
            imgdata[i] = 0;
        }

        for(int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
                int index = (y*width+x)*3;
                imgdata[index] = (unsigned char)(x * (255.0f/width));
                imgdata[index+1] = (unsigned char)(y * (255.0f/height));
                imgdata[index+2] = (unsigned char)((x+y) * (255.0f/(width+height)));
            }
        }
        CSurfCL surf = CSurfCL();
        surf.ScanImage24Bit(width, height, imgdata,  0.0002f, 5, 2, false);
        float* data = new float[surf.GetLastHeight() * surf.GetLastWidth()];
        surf.DebugGetIntegralImage(data);

        float res = data[height * width - 1];
        delete[] data;
        delete[] imgdata;

        std::cout<<"calculated result: \t" << res << std::endl;

        TS_ASSERT(res == 497484.03f || res == 497484.00f || res == 497483.78f); 
        std::cout<<"End :"<<"testIntegralImage24Bit"<<std::endl;
    }
    void testIntegralImage32Bit( void )
    {
        std::cout<<std::endl<<"Start:"<<"testIntegralImage32Bit"<<std::endl;
        const int width = 1000;
        const int height = 1000;

        const size_t size = width*height*4;
        unsigned char* imgdata = new unsigned char[size];
        for(int i = 0; i < size; i++)
        {
            imgdata[i] = 0;
        }

        for(int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
                int index = (y*width+x)*4;
                imgdata[index] = (unsigned char)(x * (255.0f/width));
                imgdata[index+1] = (unsigned char)(y * (255.0f/height));
                imgdata[index+2] = (unsigned char)((x+y) * (255.0f/(width+height)));
            }
        }
        CSurfCL surf = CSurfCL();
        surf.ScanImage32Bit(width, height, imgdata,  0.0002f, 5, 2, false);
        float* data = new float[surf.GetLastHeight() * surf.GetLastWidth()];
        surf.DebugGetIntegralImage(data);

        float res = data[height * width - 1];
        delete[] data;
        delete[] imgdata;

        std::cout<<"calculated result: \t" << res << std::endl;

        TS_ASSERT(res == 497484.03f || res == 497484.00f || res == 497483.78f); 
        std::cout<<"End :"<<"testIntegralImage32Bit"<<std::endl;
    }
	
	void testIPoints2( void )
    {

        std::cout<<std::endl<<"Start:"<<"testIPoints2"<<std::endl;
        CSurfCL surf = CSurfCL();
		surf.DebugScanFromBitmap(DATA_DIRECTORY"/cross.bmp", 0.0002f, 5, 2, false);
        SIpoint* Points = surf.GetIPointsLink();
        int count = surf.GetIPointsCount();
        int Width;
        int Height;
        unsigned char* ImageData;
        CSurfCL::DebugReadBitmap(DATA_DIRECTORY"/cross_.bmp",Width, Height, &ImageData);
        int hits = 0;
        for(int i = 0; i < count; i++)
        {
            int x = (int)Points[i].x;
            int y = (int)Points[i].y;
            int b = 0;
            int r = 0;
            for(int ix = -1; ix < 2; ix++)
            {
                for(int iy = -1; iy < 2; iy++)
                {
                    b += ImageData[((y + iy) * Width + x + ix) * 3];
                    r += ImageData[((y + iy) * Width + x + ix) * 3 + 2];
                }
            }
            

            if( Points[i].laplacian > 0)
            {
                if(b == 255)
                {
                    hits++;
                    std::cout<<"Hit : x: " << x <<" y: " << y << " laplacian" << std::endl;
                }
                else
                {
                    std::cout<<"Miss: x: " << x <<" y: " << y << " laplacian" << std::endl;
                }
            }
            else
            {
                if(r == 255)
                {
                    hits++;
                    std::cout<<"Hit : x: " << x <<" y: " << y << " response" << std::endl;
                }
                else
                {
                    std::cout<<"Miss: x: " << x <<" y: " << y << " response" << std::endl;
                }
            }

        }
        delete[] ImageData;
        TS_ASSERT_EQUALS(hits, 10);
        std::cout<<"End :"<<"testIPoints2"<<std::endl;
    }
	void testIPoints( void )
    {
        std::cout<<std::endl<<"Start:"<<"testIPoints"<<std::endl;
        CSurfCL surf = CSurfCL();

		surf.DebugScanFromBitmap(DATA_DIRECTORY"/pic_carsten.bmp", 0.0002f, 5, 2, false);
        SIpoint* Points = surf.GetIPointsLink();
        int count = surf.GetIPointsCount();

		TS_ASSERT_EQUALS(count, 1153);
		TS_ASSERT_EQUALS((int)surf.GetIPointsLink()[0].x, 593);
		TS_ASSERT_EQUALS((int)surf.GetIPointsLink()[0].y, 21);
    }

    void testIntegralImage2( void )
    {
        std::cout<<std::endl<<"Start:"<<"testIntegralImage2"<<std::endl;
        const int width = 1000;
        const int height = 1000;
        const int count = 100;
        const size_t size = width*height*3;
        unsigned char* imgdata = new unsigned char[size];
        for(int i = 0; i < size; i++)
        {
            imgdata[i] = 0;
        }

        for(int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
                int index = (y*width+x)*3;
                imgdata[index] = (unsigned char)(x * (255.0f/width));
                imgdata[index+1] = (unsigned char)(y * (255.0f/height));
                imgdata[index+2] = (unsigned char)((x+y) * (255.0f/(width+height)));
            }
        }
        CSurfCL surf = CSurfCL();
        

        float* data = new float[size/3];

        surf.DebugResizeBuffer(width,height);
        surf.DebugCreateIntegralImage(imgdata);
        surf.DebugGetIntegralImage(data);
        float result = data[height * width - 1];

        for( int i = 0; i < count-1; i++)
        {
            surf.DebugCreateIntegralImage(imgdata);
            surf.DebugGetIntegralImage(data);
            TS_ASSERT_EQUALS(result, data[height * width - 1]);
        }

        delete[] data;
        delete[] imgdata;
        std::cout<<"End :"<<"testIntegralImage2"<<std::endl;
    }
	void testIntegralImageBMPLoad( void )
    {
        std::cout<<std::endl<<"Start:"<<"testIntegralImageBMPLoad"<<std::endl;
        CSurfCL surf = CSurfCL();
        surf.DebugScanFromBitmap(DATA_DIRECTORY"/testfile.bmp", 0.0002f, 5, 2, false);
        surf.DebugPrintIntegralImage();
        float* data = new float[surf.GetLastHeight() * surf.GetLastWidth()];
        surf.DebugGetIntegralImage(data);
		
        float lastVal = data[4*5+4];
		
        delete[] data;
		//ati                    //nvidia
        TS_ASSERT(lastVal == 20.301529f || lastVal == 20.301527f); 
        std::cout<<"End :"<<"testIntegralImageBMPLoad"<<std::endl;
    }

	
	
};

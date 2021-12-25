

#include <cv.h>
#include <highgui.h>
#include <csurfcl.h>
#include <ctime>

// Roundfuncation for float our values war just plus but for safity it also handels minus
int fRound(float _Value)
{
    if (_Value < 0)
    {
        return (int)(_Value - 0.5);
    } else {
        return (int)(_Value + 0.5);
    }
}

// Draw the FPS figure on the image (requires at least 2 calls)
void drawFPS(IplImage *img)
{
  static int counter = 0;
  static clock_t t;
  static float fps;
  char fps_text[20];
  CvFont font;
  cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, 1.0,1.0,0,2);

  // Add fps figure (every 10 frames)
  if (counter > 10)
  {
    fps = (10.0f/(clock()-t) * CLOCKS_PER_SEC);
    t=clock(); 
    counter = 0;
  }

  // Increment counter
  ++counter;

  // Get the figure as a string
  sprintf(fps_text,"FPS: %.2f",fps);

  // Draw the string on the image
  cvPutText (img,fps_text,cvPoint(10,25), &font, cvScalar(255,255,0));
}

void drawIpoints(IplImage *img, SIpoint *ipts, int _count)
{
    SIpoint *ipt = NULL;
    float s, o;
    int r1, c1, r2, c2, lap;

    for(unsigned int i = 0; i < _count; i++) 
    {
        s = (2.5f * ipts[i].scale);
        o = ipts[i].orientation;
        lap = ipts[i].laplacian;
        r1 = fRound(ipts[i].y);
        c1 = fRound(ipts[i].x);
        c2 = fRound(s * cos(o)) + c1;
        r2 = fRound(s * sin(o)) + r1;

        if (o) // Green line indicates orientation
            cvLine(img, cvPoint(c1, r1), cvPoint(c2, r2), cvScalar(0, 255, 0));
        else  // Green dot if using upright version
            cvCircle(img, cvPoint(c1,r1), 1, cvScalar(0, 255, 0),-1);

        if (lap == 1)
        { // Blue circles indicate dark blobs on light backgrounds
            cvCircle(img, cvPoint(c1,r1), fRound(s), cvScalar(255, 0, 0),1);
        }
        else if (lap == 0)
        { // Red circles indicate light blobs on dark backgrounds
            cvCircle(img, cvPoint(c1,r1), fRound(s), cvScalar(0, 0, 255),1);
        }
        else if (lap == 9)
        { // Red circles indicate light blobs on dark backgrounds
            cvCircle(img, cvPoint(c1,r1), fRound(s), cvScalar(0, 255, 0),1);
        }
    }
}

    int main(int argc, char* argv[])
    {
        CSurfCL surf = CSurfCL();
        // oder: cvCaptureFromCAM( 0 );
        CvCapture* capture = cvCaptureFromCAM( 0 ); //cvCaptureFromAVI("/users/carsten/Desktop/TBBT_sampel.avi");
		
		cvNamedWindow("Proccessed Video");
        IplImage* img = 0;
		IplImage* object_gray = NULL;
		
        SIpoint* ipts;
        unsigned int FrameCount = 0;
        unsigned int PointCount = 0;
        cvGrabFrame(capture);
        img=cvRetrieveFrame(capture);
        
        surf.DebugResizeBuffer(img->width, img->height);

		img=cvRetrieveFrame(capture, 1);
		if (object_gray == NULL) object_gray = cvCreateImage(cvGetSize(img), 8, 1);
		
		// Capture um Video aufzuzeichnen
		/*double fps = cvGetCaptureProperty(
										  capture,
										  CV_CAP_PROP_FPS
										  );
		
		CvVideoWriter *writer = cvCreateVideoWriter(
													"/users/carsten/Desktop/out.avi",
													-1,
													fps,
													cvSize(img->width, img->height)
													);
		*/
		surf.DebugResizeBuffer(img->width, img->height);
		
        while(cvGrabFrame(capture))
        {  
            img=cvRetrieveFrame(capture);
			cvCvtColor( img, object_gray, CV_BGR2GRAY );
			surf.ScanImageGreyscale(object_gray->width, object_gray->height, (unsigned char*)object_gray->imageData, 0.0004f, 4, 2, false);
			//surf.ScanImage24Bit(img->width, img->height, (unsigned char*)img->imageData, 0.004f, 4, 2, false);
			surf.CalculateSurfDescriptor();
			ipts = surf.GetIPointsLink();
			PointCount = surf.GetIPointsCount();
                //printf("Anzahl Punkte: %i\n", PointCount);

			drawIpoints(img,ipts, PointCount);
			drawFPS(img);
            
			//cvWriteFrame(writer, img);
			
            cvShowImage("Proccessed Video", img);
            cvWaitKey(10);
        }

		// alles freigeben
		//cvReleaseVideoWriter(&writer);
        cvReleaseCapture(&capture);
        return 0;
    }


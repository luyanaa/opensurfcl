%module CsOpenSurfCL


#Wrapper für SIPoint
%{
#include <SIPoint.h>
%}


//swig handle the float descriptors[64] as float* descriptors
//we have to say how to handle the float array correctly

//replaces the standard SWIGTYPE_p_float with float[]
%typemap(cstype) float [64] "float[]"

//now csharp code in the get{} part
//$imcall gets the pointer (something like this: CsOpenSurfCLPINVOKE.SIpoint_descriptors_get(swigCPtr))
%typemap(csvarout) float [64] 
%{
	get
	{
		  IntPtr cPtr = $imcall;
		  float[] outArray = new float[64];
		  Marshal.Copy(cPtr, outArray, 0, 64);
		  return outArray;
	}
%}

//remove the set part
%typemap(csvarin) float [64]
%{
%}

%include "../opensurfcl/SIPoint.h"
%extend SIpoint
{
	void Test()
	{
		//test
	}
};

%inline %{
  size_t GetSIpointByteSize()
  {
	return sizeof(SIpoint);
  }
%}


#Wrapper für CCLDevice
%{
#include <CCLDevice.h>
%}
%include "../opensurfcl/CCLDevice.h"


//Einbinden von CSurfCL.h in die von Swig erstellte cpp Datei
%{
#include <CSurfCL.h>
%}



//sollte dringend noch verbessert werden!
//typemap um bei arrays direkt mit IntPtr zu arbeiten in C#
%define CSHARP_PTRTOPTR( CTYPE, CSTYPE )
%typemap(ctype)   CTYPE PTRTOPTR "CTYPE*"
%typemap(cstype)  CTYPE PTRTOPTR "CSTYPE"
%typemap(imtype)  CTYPE PTRTOPTR "IntPtr"
%typemap(csin)    CTYPE PTRTOPTR "$csinput"

%typemap(in)      CTYPE PTRTOPTR "$1 = $input;"
%typemap(freearg) CTYPE PTRTOPTR ""
%typemap(argout)  CTYPE PTRTOPTR ""
%enddef // CSHARP_PTRTOPTR
CSHARP_PTRTOPTR(unsigned char, IntPtr)

//wendet das oben erstellte typemap auf array "unsigned char* _ImgData" an
%apply unsigned char PTRTOPTR  {unsigned char*}

%rename(GetIPointsLinkIntern)  GetIPointsLink;
%csmethodmodifiers GetIPointsLinkIntern "private";
%typemap(cscode) CSurfCL
%{
    public SIpoint[] GetIPointsLink() 
    {
		SIpoint[] iarray = null;
        IntPtr startPtr = CsOpenSurfCLPINVOKE.CSurfCL_GetIPointsLinkIntern(swigCPtr);
		int count = GetIPointsCount();
		long size = CsOpenSurfCL.GetSIpointByteSize();
        if (startPtr != IntPtr.Zero)
        {
            iarray = new SIpoint[count];
            for (int i = 0; i < iarray.Length; i++)
            {
                iarray[i] = new SIpoint(new IntPtr(startPtr.ToInt64() + size * i), false);
            }
        }
		return iarray;
    }

    public void ScanImage(System.Drawing.Bitmap image, float _Threshold, int _Octaves, int _Initsample, bool extendedDescriptor) 
    {
        if (image.PixelFormat == System.Drawing.Imaging.PixelFormat.Format24bppRgb)
        {
            System.Drawing.Imaging.BitmapData data = image.LockBits(
                new System.Drawing.Rectangle(0, 0, image.Width, image.Height), 
                System.Drawing.Imaging.ImageLockMode.ReadOnly, 
                image.PixelFormat);
		    ScanImage24Bit(image.Width, image.Height, data.Scan0, _Threshold, _Octaves, _Initsample, extendedDescriptor);
		    image.UnlockBits(data);

        }else if(image.PixelFormat == System.Drawing.Imaging.PixelFormat.Format32bppArgb)
        {
            System.Drawing.Imaging.BitmapData data = image.LockBits(
                new System.Drawing.Rectangle(0, 0, image.Width, image.Height),
                System.Drawing.Imaging.ImageLockMode.ReadOnly,
                image.PixelFormat);
            ScanImage32Bit(image.Width, image.Height, data.Scan0, _Threshold, _Octaves, _Initsample, extendedDescriptor);
            image.UnlockBits(data);
        }else if(image.PixelFormat == System.Drawing.Imaging.PixelFormat.Format8bppIndexed)
        {
            System.Drawing.Imaging.BitmapData data = image.LockBits(
                new System.Drawing.Rectangle(0, 0, image.Width, image.Height),
                System.Drawing.Imaging.ImageLockMode.ReadOnly,
                image.PixelFormat);
            ScanImageGreyscale(image.Width, image.Height, data.Scan0, _Threshold, _Octaves, _Initsample, extendedDescriptor);
            image.UnlockBits(data);
        }else
        {
            throw new FormatException("This method supports only Format24ppRgb, Format32bppArgb and Format8bppIndexed!");
        }
    }
%}

//alle Einstellungen wurden abgeschlossen -> parsen der CSurfCL.h
%include "../opensurfcl/CSurfCL.h"

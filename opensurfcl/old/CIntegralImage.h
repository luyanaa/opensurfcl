#pragma once
#include <math.h>
#include <algorithm>
#include <fstream>
#include <assert.h>

class CIntegralImage
{
private:
    static const float cR;
    static const float cG;
    static const float cB;



public:
    float** Matrix;
    int Width;
    int Height;

    

    
    
    CIntegralImage(int width, int height);
    ~CIntegralImage();

    float*& operator[](int index)
    {
        return Matrix[index];
    }


    static CIntegralImage* FromBitmapFile(char* _Path);

    static void FromImage(unsigned char* imageData, CIntegralImage& _rResult);

    float BoxIntegral(int row, int col, int rows, int cols);

    /// <summary>
    /// Get Haar Wavelet X repsonse
    /// </summary>
    /// <param name="row"></param>
    /// <param name="column"></param>
    /// <param name="size"></param>
    /// <returns></returns>
    float HaarX(int row, int column, int size)
    {
      return BoxIntegral(row - size / 2, column, size, size / 2)
        - 1 * BoxIntegral(row - size / 2, column - size / 2, size, size / 2);
    }

    /// <summary>
    /// Get Haar Wavelet Y repsonse
    /// </summary>
    /// <param name="row"></param>
    /// <param name="column"></param>
    /// <param name="size"></param>
    /// <returns></returns>
    float HaarY(int row, int column, int size)
    {
      return BoxIntegral(row, column - size / 2, size / 2, size)
        - 1 * BoxIntegral(row - size / 2, column - size / 2, size / 2, size);
    }
  };
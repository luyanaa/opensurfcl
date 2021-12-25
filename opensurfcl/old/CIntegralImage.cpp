#include "CIntegralImage.h"

const float CIntegralImage::cR(0.2989f);
const float CIntegralImage::cG(0.5870f);
const float CIntegralImage::cB(0.1140f);

CIntegralImage::CIntegralImage(int width, int height)// : cR(0.2989f)
{
    this->Width = width;
    this->Height = height;

    //initialize the multidimensional array as a whole piece
    this->Matrix = new float*[height];
    this->Matrix[0] = new float[height * width];
    for(int i = 1; i < height; i++)
    {
        this->Matrix[i] = this->Matrix[i-1] + width;
    }
    
}

CIntegralImage::~CIntegralImage()
{
    delete[] this->Matrix[0];
    delete[] this->Matrix;
}

void CIntegralImage::FromImage(unsigned char* imageData, CIntegralImage& _rResult)
{
    float rowsum = 0;
    for (int x = 0; x < _rResult.Width; x++)
    {
        float r = imageData[x*3 + 2];
        float g = imageData[x*3 + 1];
        float b = imageData[x*3];
        rowsum += (cR * r + cG * g + cB * b) / 255.0f;
        _rResult[0][x] = rowsum;
    }
    for (int y = 1; y < _rResult.Height; y++)
    {
        rowsum = 0;
        for (int x = 0; x < _rResult.Width; x++)
        {
            float r = imageData[(y*_rResult.Width + x) * 3 + 2];
            float g = imageData[(y*_rResult.Width + x) * 3 + 1];
            float b = imageData[(y*_rResult.Width + x) * 3];
            
            rowsum += (cR * r + cG * g + cB * b) / 255.0f;
            
            // integral image is rowsum + value above
            _rResult[y][x] = rowsum + _rResult[y - 1][x];
        }
    }
}

CIntegralImage* CIntegralImage::FromBitmapFile(char* _Path)
{
    std::fstream Instream(_Path, std::fstream::in | std::fstream::binary);
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
        add = 4 - (width * 3)%4;

    for( int y = 0; y < height; y++)
    {
        Instream.read((char*)&pixels[0] + y * width, width * 3);
        int nextPos = Instream.tellg();
        nextPos += add;
        Instream.seekg(nextPos);
    }

    //handle delete later
    CIntegralImage* result = new CIntegralImage(height, width);
    FromImage(pixels, *result);

    delete[] pixels;

    return result;
}

float CIntegralImage::BoxIntegral(int row, int col, int rows, int cols)
{
    // The subtraction by one for row/col is because row/col is inclusive.
    int r1 = std::min(row, Height) - 1;
    int c1 = std::min(col, Width) - 1;
    int r2 = std::min(row + rows, Height) - 1;
    int c2 = std::min(col + cols, Width) - 1;

    float A = 0, B = 0, C = 0, D = 0;
    if (r1 >= 0 && c1 >= 0) A = Matrix[r1][c1];
    if (r1 >= 0 && c2 >= 0) B = Matrix[r1][c2];
    if (r2 >= 0 && c1 >= 0) C = Matrix[r2][c1];
    if (r2 >= 0 && c2 >= 0) D = Matrix[r2][c2];

    return std::max(0.0f, A - B - C + D);
}
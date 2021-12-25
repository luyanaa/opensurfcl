#ifndef IPOINT_H
#define IPOINT_H

class IPoint
{
public:
    float x,y;
    float scale;
    float response;
    float orientation;
    int laplacian;
    int descriptorLength;
    float* descriptor;

    
    IPoint();
    ~IPoint();

    void SetDescriptorLength(int Size);
};

#endif
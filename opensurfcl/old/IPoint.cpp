#include "IPoint.h"

IPoint::IPoint()
{
}

IPoint::~IPoint()
{

}

void IPoint::SetDescriptorLength(int Size)
{
    descriptorLength = Size;
    descriptor = new float[Size];
}
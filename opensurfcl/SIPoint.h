#pragma once
struct SIpoint
{
	float x,y;
    float scale;
    float response;
    float orientation;
    int laplacian;
	int descriptor_length;
	float descriptors[64];
};

// kleines Struct für weniger Overhead
typedef struct
{
	float x,y;
    float scale;
    int laplacian;
}  SIpointSmall;
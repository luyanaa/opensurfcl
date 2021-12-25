#pragma once
#ifdef __APPLE__
	#include <OpenCL/OpenCL.h>
#else
	#include <CL/cl.h>
#endif

class CResponseLayer
{
protected:
	int m_width;
	int m_height;
	int m_step;
	int m_filter;
	
	float *m_responses;
	unsigned char *m_laplacian;
	
public:
	CResponseLayer(int width, int height, int step, int filter);
	unsigned char getLaplacian(int row, int column);
	unsigned char getLaplacian(int row, int column, CResponseLayer &src);
	float getResponse(int row, int column);
	float getResponse(int row, int column, CResponseLayer &src);
	int getWidth(void);
	int getHeight(void);
	int getStep(void);
	int getFilter(void);
	void setResponse(int index, float value);
	void setLaplacian(int index, unsigned char value);
	
	~CResponseLayer(void);
};

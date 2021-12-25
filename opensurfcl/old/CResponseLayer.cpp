#include "CResponseLayer.h"
#include <iostream>

CResponseLayer::CResponseLayer(int width, int height, int step, int filter)
{
	m_width = width;
	m_height = height;
	m_step = step;
	m_filter = filter;
	
	m_responses = new float[width * height];
	m_laplacian = new unsigned char[width * height];
}

unsigned char CResponseLayer::getLaplacian(int row, int column)
{
	return m_laplacian[row * m_width + column];
}

unsigned char CResponseLayer::getLaplacian(int row, int column, CResponseLayer &src)
{
	int scale = m_width / src.getWidth();
	return m_laplacian[(scale * row) * width + (scale * column)];
}

float CResponseLayer::getResponse(int row, int column)
{
	return m_responses[row * width + column];
}

float CResponseLayer::getResponse(int row, int column, CResponseLayer &src)
{
	int scale = m_width / src.getWidth();
	return m_responses[(scale * row) * width + (scale * column)];
}

int CResponseLayer::getWidth(void)
{
	return m_width;
}

int CResponseLayer::getHeight(void)
{
	return m_height;
}

int CResponseLayer::getStep(void)
{
	return m_step;
}

int CResponseLayer::getFilter(void)
{
	return m_filter;
}

void setResponse(int index, float value)
{
	assert(index < width * height);
	m_responses[index] = value;
}

void setLaplacian(int index, unsigned char value)
{
	assert(index < width * height);
	m_laplacian[index] = value;
}

CResponseLayer::~CResponseLayer(void)
{
	delete[] m_responses;
	delete[] m_laplacian;
}

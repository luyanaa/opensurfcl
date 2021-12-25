#pragma once
#ifdef __APPLE__
	#include <OpenCL/OpenCL.h>
#else
	#include <CL/cl.h>
#endif
#include <vector>
#include "CResponseLayer.h"
#include "CMatrix.h"
#include "IPoint.h"
#include "CIntegralImageCL.h"

using namespace std;

class CHessian
{
protected:
	float m_thresh;
	int m_octaves;
	int m_initsample;
	CIntegralImageCL m_img;
	vector<IPoint> m_ipts;
	vector<CResponseLayer> m_responseMap;
	
public:
	CHessian(float thresh, int octaves, int init_sample, CIntegralImageCL &img);
	~CHessian(void);
	vector<IPoint> getIPoints(void);
	
protected:
	void buildResponseMap(void);
	void buildResponseLayer(CResponseLayer &rl);
	bool isExtremum(int r, int c, CResponseLayer &t, CResponseLayer &m, CResponseLayer &b);
	void interpolateExtremum(int r, int c, CResponseLayer &t, CResponseLayer &m, CResponseLayer &b);
	vector<double> BuildDerivative(int r, int c, CResponseLayer &t, CResponseLayer &m, CResponseLayer &b);
	vector<double> BuildHessian(int r, int c, CResponseLayer &t, CResponseLayer &m, CResponseLayer &b);
	
};

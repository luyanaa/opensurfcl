#pragma once
#ifdef __APPLE__
	#include <OpenCL/OpenCL.h>
#else
	#include <CL/cl.h>
#endif
#include <vector>

using namespace std;

class CMatrix
{
protected:
	vector<double> m_values;
	int m_m;
	int m_n;
	
public:
	CMatrix(vector<double> values, int m, int n);
	~CHessian(void);
	CMatrix Inverse();
	const CMatrix operator*(int const& lhs, CMatrix const& rhs);
	const CMatrix operator*(CMatrix const& lhs, CMatrix const& rhs);
	int getM();
	int getN();
	double getElem(int m, int n);
};

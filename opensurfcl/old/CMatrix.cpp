#include "CMatrix.h"
#include <iostream>
#include "math.h"
#include <assert.h>

CMatrix::CMatrix(vector<double> values, int m, int n)
{
	m_values = values;
	m_m = m;
	m_n = n;
}

CMatrix::CMatrix(void)
{

}

CMatrix CMatrix::Inverse()
{
	// calculate the inverse matrix
}

const CMatrix CMatrix::operator*(CMatrix const& lhs, CMatrix const& rhs)
{
	// multiply two matrices
}

const CMatrix CMatrix::operator*(int const& lhs, CMatrix const& rhs)
{
	vector<double> newVal;
	for (int i = 0; i < rhs.getN(); ++i) {
		for (int j = 0; j < rhs.getM(); ++j) {
			newVal.push_back(rhs.getElem(j, i) * lhs);
		}
	}
	
	return new CMatrix(newVal, m_m, m_n);
}

int CMatrix::getM()
{
	return m_m;
}

int CMatrix::getN()
{
	return m_n;
}

double CMatrix::getElem(int m, int n)
{
	assert((m <= m_m) && (n <= m_n));
	return m_values[m_n * m + n];
}
#include "CHessian.h"
#include <iostream>
#include "math.h"


CHessian::CHessian(float thresh, int octaves, int init_sample, CIntegralImageCL &img)
{
	m_thresh = thresh;
	m_octaves = octaves;
	m_initsample = init_sample;
	m_img = img;
}

CHessian::~CHessian(void)
{

}

vector<IPoint> CHessian::getIPoints(void)
{
	// filter map
	int filter_map[20] = {0,1,2,3,1,3,4,5,3,5,6,7,5,7,8,9,7,9,10,11};
	
	// clear the vector 
	m_ipts.clear();
	
	// build response map
	buildResponseMap();
	
	// get response layers
	CResponseLayer b;
	CResponseLayer m;
	CResponseLayer t;
	
	for (int i = 0; i < m_octaves; ++i) {
		for (int j = 0; j <= 1; ++j) {
			b = m_responseMap[filter_map[4 * i + j]];
			m = m_responseMap[filter_map[4 * i + j + 1]];
			t = m_responseMap[filter_map[4 * i + j + 2]];
			
			for (int r = 0; r < t.getHeight(); ++r) {
				for (int c = 0; c < t.getHeight(); ++c) {
					if (isExtremum(r, c, t, m, b)) {
						interpolateExtremum(r, c, t, m, b);
					}
				}
			}
		}
	}
}

void CHessian::buildResponseMap(void)
{
	// clear existing values
	m_responseMap.clear();
	
	// image attributes
	int w = (m_img.getWidth() / m_initsample);
	int h = (m_img.getHeight() / m_initsample);
	int s = m_initsample;
	
	// calculate the hessian values
	if (m_octaves >= 1)
	{
		m_responseMap.push_back(new CResponseLayer(w, h, s,  9));
        m_responseMap.push_back(new CResponseLayer(w, h, s, 15));
        m_responseMap.push_back(new CResponseLayer(w, h, s, 21));
        m_responseMap.push_back(new CResponseLayer(w, h, s, 27));
	}
	
	if (m_octaves >= 2)
	{
        m_responseMap.push_back(new CResponseLayer(w / 2, h / 2, s * 2, 39));
        m_responseMap.push_back(new CResponseLayer(w / 2, h / 2, s * 2, 51));
	}
	
	if (m_octaves >= 3)
	{
        m_responseMap.push_back(new CResponseLayer(w / 4, h / 4, s * 4, 75));
        m_responseMap.push_back(new CResponseLayer(w / 4, h / 4, s * 4, 99));
	}
	
	if (m_octaves >= 4)
	{
        m_responseMap.push_back(new CResponseLayer(w / 8, h / 8, s * 8, 147));
        m_responseMap.push_back(new CResponseLayer(w / 8, h / 8, s * 8, 195));
	}
	
	if (m_octaves >= 5)
	{
        m_responseMap.push_back(new CResponseLayer(w / 16, h / 16, s * 16, 291));
        m_responseMap.push_back(new CResponseLayer(w / 16, h / 16, s * 16, 387));
	}
	
	// Extract responses from the image
	for (int i = 0; i < (int)responseMap.size(); ++i)
	{
        buildResponseLayer(m_responseMap[i]);
	}
}

void CHessian::buildResponseLayer(CResponseLayer &rl)
{
	int step = rl.getStep();                      // step size for this filter
	int b = (rl.getFilter() - 1) / 2 + 1;         // border for this filter
	int l = rl.getFilter() / 3;                   // lobe for this filter (filter size / 3)
	int w = rl.getFilter;                       // filter size
	float inverse_area = 1f / (w * w);       // normalisation factor
	float Dxx, Dyy, Dxy;
	
	for (int r, c, ar = 0, index = 0; ar < rl.getHeight(); ++ar)
	{
        for (int ac = 0; ac < rl.getWidth(); ++ac, index++)
        {
			// get the image coordinates
			r = ar * step;
			c = ac * step;
			
			// Compute response components
			Dxx = img.BoxIntegral(r - l + 1, c - b, 2 * l - 1, w)
			- img.BoxIntegral(r - l + 1, c - l / 2, 2 * l - 1, l) * 3;
			Dyy = img.BoxIntegral(r - b, c - l + 1, w, 2 * l - 1)
			- img.BoxIntegral(r - l / 2, c - l + 1, l, 2 * l - 1) * 3;
			Dxy = + img.BoxIntegral(r - l, c + 1, l, l)
			+ img.BoxIntegral(r + 1, c - l, l, l)
			- img.BoxIntegral(r - l, c - l, l, l)
			- img.BoxIntegral(r + 1, c + 1, l, l);
			
			// Normalise the filter responses with respect to their size
			Dxx *= inverse_area;
			Dyy *= inverse_area;
			Dxy *= inverse_area;
			
			// Get the determinant of hessian response & laplacian sign
			rl.setResponse(index, (Dxx * Dyy - 0.81f * Dxy * Dxy));
			rl.setLaplacian(index, (unsigned char)(Dxx + Dyy >= 0 ? 1 : 0));
		}
	}	
}

bool CHessian::isExtremum(int r, int c, CResponseLayer &t, CResponseLayer &m, CResponseLayer &b)
{
	// bounds check
	int layerBorder = (t.getFilter() + 1) / (2 * t.getStep());
	if (r <= layerBorder || r >= t.getHeight() - layerBorder || c <= layerBorder || c >= t.getWidth() - layerBorder)
        return false;
	
	// check the candidate point in the middle layer is above thresh 
	float candidate = m.getResponse(r, c, t);
	if (candidate < m_thresh)
        return false;
	
	for (int rr = -1; rr <= 1; ++rr)
	{
        for (int cc = -1; cc <= 1; ++cc)
        {
			// if any response in 3x3x3 is greater candidate not maximum
			if (t.getResponse(r + rr, c + cc) >= candidate ||
				((rr != 0 || cc != 0) && m.getResponse(r + rr, c + cc, t) >= candidate) ||
				b.getResponse(r + rr, c + cc, t) >= candidate)
			{
				return false;
			}
        }
	}
	return true;
}

void CHessian::interpolateExtremum(int r, int c, CResponseLayer &t, CResponseLayer &m, CResponseLayer &b)
{
	CMatrix D = CMatrix.Create(BuildDerivative(r, c, t, m, b), 1, 3);
	CMatrix H = CMatrix.Create(BuildHessian(r, c, t, m, b), 3, 3);
	CMatrix Hi = H.Inverse();
	CMatrix Of = -1 * Hi * D;
	
	// get the offsets from the interpolation
	double[] O = { Of[0, 0], Of[1, 0], Of[2, 0] };
	
	// get the step distance between filters
	int filterStep = (m.getFilter() - b.getFilter());
	
	// If point is sufficiently close to the actual extremum
	if (fabs(O[0]) < 0.5f && fabs(O[1]) < 0.5f && fabs(O[2]) < 0.5f)
	{
        IPoint ipt = new IPoint();
        ipt.x = (float)((c + O[0]) * t.getStep());
        ipt.y = (float)((r + O[1]) * t.getStep());
        ipt.scale = (float)((0.1333f) * (m.getFilter() + O[2] * filterStep));
        ipt.laplacian = (int)(m.getLaplacian(r,c,t));
		m_ipts.push_back(ipt);
      }
}

vector<double> CHessian::BuildDerivative(int r, int c, CResponseLayer &t, CResponseLayer &m, CResponseLayer &b)
{
	double dx, dy, ds;
	
	dx = (m.getResponse(r, c + 1, t) - m.getResponse(r, c - 1, t)) / 2f;
	dy = (m.getResponse(r + 1, c, t) - m.getResponse(r - 1, c, t)) / 2f;
	ds = (t.getResponse(r, c) - b.getResponse(r, c, t)) / 2f;
	
	vector<double> retVal;
	retVal.push_back(dx);
	retVal.push_back(dy);
	retVal.push_back(ds);
	
	return retVal;	
}

vector<double> CHessian::BuildHessian(int r, int c, CResponseLayer &t, CResponseLayer &m, CResponseLayer &b)
{
	double v, dxx, dyy, dss, dxy, dxs, dys;
	
	v = m.getResponse(r, c, t);
	dxx = m.getResponse(r, c + 1, t) + m.getResponse(r, c - 1, t) - 2 * v;
	dyy = m.getResponse(r + 1, c, t) + m.getResponse(r - 1, c, t) - 2 * v;
	dss = t.getResponse(r, c) + b.getResponse(r, c, t) - 2 * v;
	dxy = (m.getResponse(r + 1, c + 1, t) - m.getResponse(r + 1, c - 1, t) -
		   m.getResponse(r - 1, c + 1, t) + m.getResponse(r - 1, c - 1, t)) / 4f;
	dxs = (t.getResponse(r, c + 1) - t.getResponse(r, c - 1) -
		   b.getResponse(r, c + 1, t) + b.getResponse(r, c - 1, t)) / 4f;
	dys = (t.getResponse(r + 1, c) - t.getResponse(r - 1, c) -
		   b.getResponse(r + 1, c, t) + b.getResponse(r - 1, c, t)) / 4f;
	
	vector<double> retVal;
	retVal.push_back(dxx);
	retVal.push_back(dxy);
	retVal.push_back(dxs);
	retVal.push_back(dxy);
	retVal.push_back(dyy);
	retVal.push_back(dys);
	retVal.push_back(dxs);
	retVal.push_back(dys);
	retVal.push_back(dss);
	
	return retVal;
}
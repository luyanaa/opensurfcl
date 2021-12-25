/// This code is based on the orginal OpenSurf Implementation by Chris Evans!
/// http://code.google.com/p/opensurf1

//Struct für IPoint
typedef struct
{
	float x,y;
    float scale;
    float response;
    float orientation;
    int laplacian;
	int descriptor_length;
	float descriptors[64];
}  SIpoint;

// kleines Struct für weniger Overhead
typedef struct
{
	float x,y;
    float scale;
    int laplacian;
}  SIpointSmall;

// Struct für ResponseLayer
typedef struct
{
	//float* responses;
	int responses;
	//unsigned int* laplacian;
	int laplacian;
	int width;
	int height;
	int step;
	int filter;
}  SResponseLayer;

__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

float getResponse(int row, int column, int t, int i, int width, int height, __global float* rBuffer, __global SResponseLayer* responselayers)
{
	// wenn i < 0 dann will ich die Response von t
	// wenn i > 0 dann will ich die Response von i mit t als source
	if (i >= 0)
	{
		int scale = responselayers[i].width / responselayers[t].width;
		return rBuffer[(i*width*height) + ((scale * row) * responselayers[i].width + (scale * column))];
	} else {
		return rBuffer[(t*width*height) + (row * responselayers[t].width + column)];
	}
}

float getResponseImage(int row, int column, int t, int i, int width, int height, __read_only image2d_t rBuffer,
						__global SResponseLayer* responselayers)
{
	// wenn i < 0 dann will ich die Response von t
	// wenn i > 0 dann will ich die Response von i mit t als source
	if (i >= 0)
	{
		int scale = responselayers[i].width / responselayers[t].width;
		
		int2 pos = {scale * column, i * height + scale * row};
		return  read_imagef(rBuffer, sampler, pos).x;
	} else {
		int2 pos = {column, t * height + row};
		return  read_imagef(rBuffer, sampler, pos).x;
	}
}

void BuildDerivative(int r, int c, int t, int m, int b, int width, int height, __global float* rBuffer, float *derivates, __global SResponseLayer* responselayers)
{
	float dx, dy, ds;
	
	// Ableitung nach x
	dx = (getResponse(r, c + 1, t, m, width, height, rBuffer, responselayers)
		- getResponse(r, c - 1, t, m, width, height, rBuffer, responselayers)) / 2.0f;
		
	// Ableitung nach y
	dy = (getResponse(r + 1, c, t, m, width, height, rBuffer, responselayers)
		- getResponse(r - 1, c, t, m, width, height, rBuffer, responselayers)) / 2.0f;
		
	ds = (getResponse(r + 1, c, t, -1, width, height, rBuffer, responselayers)
		- getResponse(r - 1, c, t, b, width, height, rBuffer, responselayers)) / 2.0f;
	
	derivates[0] = dx;
	derivates[1] = dy;
	derivates[2] = ds;
}

void BuildDerivativeImage(int r, int c, int t, int m, int b, int width, int height, __read_only image2d_t rBuffer, float *derivates, __global SResponseLayer* responselayers)
{
	float dx, dy, ds;
	
	// Ableitung nach x
	dx = (getResponseImage(r, c + 1, t, m, width, height, rBuffer, responselayers)
		- getResponseImage(r, c - 1, t, m, width, height, rBuffer, responselayers)) / 2.0f;
		
	// Ableitung nach y
	dy = (getResponseImage(r + 1, c, t, m, width, height, rBuffer, responselayers)
		- getResponseImage(r - 1, c, t, m, width, height, rBuffer, responselayers)) / 2.0f;
		
	ds = (getResponseImage(r + 1, c, t, -1, width, height, rBuffer, responselayers)
		- getResponseImage(r - 1, c, t, b, width, height, rBuffer, responselayers)) / 2.0f;
	
	derivates[0] = dx;
	derivates[1] = dy;
	derivates[2] = ds;
}

void BuildHessian(int r, int c, int t, int m, int b, int width, int height, __global float* rBuffer, __global SResponseLayer* responselayers, float *hessian)
{
	float v, dxx, dyy, dss, dxy, dxs, dys;
	
	v = getResponse(r, c, t, m, width, height, rBuffer, responselayers);
	dxx = getResponse(r, c + 1, t, m, width, height, rBuffer, responselayers) + getResponse(r, c - 1, t, m, width, height, rBuffer, responselayers) - 2 * v;
	dyy = getResponse(r + 1, c, t, m, width, height, rBuffer, responselayers) + getResponse(r - 1, c, t, m, width, height, rBuffer, responselayers) - 2 * v;
	dss = getResponse(r, c, t, -1, width, height, rBuffer, responselayers) + getResponse(r, c, t, b, width, height, rBuffer, responselayers) - 2 * v;
	dxy = (getResponse(r + 1, c + 1, t, m, width, height, rBuffer, responselayers) - getResponse(r + 1, c - 1, t, m, width, height, rBuffer, responselayers) -
		   getResponse(r - 1, c + 1, t, m, width, height, rBuffer, responselayers) + getResponse(r - 1, c - 1, t, m, width, height, rBuffer, responselayers)) / 4.0f;
	dxs = (getResponse(r, c + 1, t, -1, width, height, rBuffer, responselayers) - getResponse(r, c - 1, t, -1, width, height, rBuffer, responselayers) -
		   getResponse(r, c + 1, t, b, width, height, rBuffer, responselayers) + getResponse(r, c - 1, t, b, width, height, rBuffer, responselayers)) / 4.0f;
	dys = (getResponse(r + 1, c, t, -1, width, height, rBuffer, responselayers) - getResponse(r - 1, c, t, -1, width, height, rBuffer, responselayers) -
		   getResponse(r + 1, c, t, b, width, height, rBuffer, responselayers) + getResponse(r - 1, c, t, b, width, height, rBuffer, responselayers)) / 4.0f;
	
	
	hessian[0] = dxx;
	hessian[1] = dxy;
	hessian[2] = dxs;
	
	hessian[3] = dxy;
	hessian[4] = dyy;
	hessian[5] = dys;
	
	hessian[6] = dxs;
	hessian[7] = dys;
	hessian[8] = dss;
	
}

void BuildHessianImage(int r, int c, int t, int m, int b, int width, int height,__read_only image2d_t rBuffer, __global SResponseLayer* responselayers, float *hessian)
{
	float v, dxx, dyy, dss, dxy, dxs, dys;
	
	v = getResponseImage(r, c, t, m, width, height, rBuffer, responselayers);
	dxx = getResponseImage(r, c + 1, t, m, width, height, rBuffer, responselayers) + getResponseImage(r, c - 1, t, m, width, height, rBuffer, responselayers) - 2 * v;
	dyy = getResponseImage(r + 1, c, t, m, width, height, rBuffer, responselayers) + getResponseImage(r - 1, c, t, m, width, height, rBuffer, responselayers) - 2 * v;
	dss = getResponseImage(r, c, t, -1, width, height, rBuffer, responselayers) + getResponseImage(r, c, t, b, width, height, rBuffer, responselayers) - 2 * v;
	dxy = (getResponseImage(r + 1, c + 1, t, m, width, height, rBuffer, responselayers) - getResponseImage(r + 1, c - 1, t, m, width, height, rBuffer, responselayers) -
		   getResponseImage(r - 1, c + 1, t, m, width, height, rBuffer, responselayers) + getResponseImage(r - 1, c - 1, t, m, width, height, rBuffer, responselayers)) / 4.0f;
	dxs = (getResponseImage(r, c + 1, t, -1, width, height, rBuffer, responselayers) - getResponseImage(r, c - 1, t, -1, width, height, rBuffer, responselayers) -
		   getResponseImage(r, c + 1, t, b, width, height, rBuffer, responselayers) + getResponseImage(r, c - 1, t, b, width, height, rBuffer, responselayers)) / 4.0f;
	dys = (getResponseImage(r + 1, c, t, -1, width, height, rBuffer, responselayers) - getResponseImage(r - 1, c, t, -1, width, height, rBuffer, responselayers) -
		   getResponseImage(r + 1, c, t, b, width, height, rBuffer, responselayers) + getResponseImage(r - 1, c, t, b, width, height, rBuffer, responselayers)) / 4.0f;
	
	
	hessian[0] = dxx;
	hessian[1] = dxy;
	hessian[2] = dxs;
	
	hessian[3] = dxy;
	hessian[4] = dyy;
	hessian[5] = dys;
	
	hessian[6] = dxs;
	hessian[7] = dys;
	hessian[8] = dss;
	
}


void inverse3(float* matrix)
{
	// nach http://www.wikihow.com/Inverse-a-3X3-Matrix
	// Determinante bestimmen
	// 	0a	3b	6c
	//	1d	4e	7f
	//	2g	5h	8i
	float det = matrix[0] * matrix[4] * matrix[8] + 
				matrix[3] * matrix[7] * matrix[2] +
				matrix[6] * matrix[1] * matrix[5] -
				matrix[2] * matrix[4] * matrix[6] -
				matrix[5] * matrix[7] * matrix[0] -
				matrix[8] * matrix[1] * matrix[3];
	
	// Transponieren
	float mt[9] = {	matrix[0], matrix[3], matrix[6],
					matrix[1], matrix[4], matrix[7],
					matrix[2], matrix[5], matrix[8]};
	
	float adjmt[9] = { 	(matrix[4] * matrix[8] - matrix[5] * matrix[7]), 
						-(matrix[3] * matrix[8] - matrix[5] * matrix[6]),
						(matrix[3] * matrix[7] - matrix[4] * matrix[6]),
		
						-(matrix[1] * matrix[8] - matrix[2] * matrix[7]),
						(matrix[0] * matrix[8] - matrix[2] * matrix[6]),
						-(matrix[0] * matrix[7] - matrix[1] * matrix[6]),
		
						(matrix[1] * matrix[5] - matrix[4] * matrix[2]),
						-(matrix[0] * matrix[5] - matrix[2] * matrix[3]),
						(matrix[0] * matrix[4] - matrix[1] * matrix[3]) };
						
	for (int i = 0; i < 9; i++)
	{
		matrix[i] = 1/det * adjmt[i];
	}	
}

void multiply3(float factor, float* matrix, float* vector)
{
	float retVal[3];
	
	// raus kommt ein vector
	// Matrix
	// 	0a	3b	6c
	//	1d	4e	7f
	//	2g	5h	8i
	
	// Vector
	//  0
	//  1
	//  2
	
	// 0 3 6 * 0 1 2
	// 1 4 7 * 0 1 2
	// 2 5 8 * 0 1 2
	retVal[0] = matrix[0] * vector[0] + matrix[3] * vector[1] + matrix[6] * vector[2];
	retVal[1] = matrix[1] * vector[0] + matrix[4] * vector[1] + matrix[7] * vector[2];
	retVal[2] = matrix[2] * vector[0] + matrix[5] * vector[1] + matrix[8] * vector[2];
	
	vector[0] = factor * retVal[0];
	vector[1] = factor * retVal[1];
	vector[2] = factor * retVal[2];
}

char getLaplacian(int row, int column, int m, int src, int width, int height,
						__global SResponseLayer* responselayers, __global unsigned char* lBuffer )
{
	int scale = responselayers[m].width / responselayers[src].width;
	return (char)lBuffer[(m*width*height) + ((scale * row) * responselayers[m].width + (scale * column))];
}

char getLaplacianImage(int row, int column, int m, int src, int width, int height,
						__global SResponseLayer* responselayers, __read_only image2d_t lBuffer)
{
	int scale = responselayers[m].width / responselayers[src].width;
	int2 pos = {scale * column, m * height + scale * row};
	
	return  read_imageui(lBuffer, sampler, pos).x;
}

bool interpolateExtremum(int r, int c, int t, int m, int b, int width, int height, unsigned int ptIndex,
						__global SResponseLayer* responselayers, __global float* rBuffer, 
						__global unsigned char* lBuffer,
						__global SIpointSmall* points)
{
	float derivates[3];
	float hessian[9];
	
	BuildDerivative(r, c, t, m, b, width, height, rBuffer, derivates, responselayers);
	BuildHessian(r, c, t, m, b, width, height, rBuffer, responselayers, hessian);
	inverse3(hessian);
	
	multiply3(-1.0f, hessian, derivates);

	// get the step distance between filters
	int filterStep = (responselayers[m].filter - responselayers[b].filter);
	
	// If point is sufficiently close to the actual extremum
	if (fabs(derivates[0]) < 0.5f && fabs(derivates[1]) < 0.5f && fabs(derivates[2]) < 0.5f)
	{
        points[ptIndex].x = (float)((c + derivates[0]) * responselayers[t].step);
        points[ptIndex].y = (float)((r + derivates[1]) * responselayers[t].step);
        points[ptIndex].scale = (float)((0.1333f) * (responselayers[m].filter + derivates[2] * filterStep));
        points[ptIndex].laplacian = getLaplacian(r,c,m,t,width, height, responselayers, lBuffer);

		return true;
	}
    
	return false;
}

bool interpolateExtremumImage(int r, int c, int t, int m, int b, int width, int height, unsigned int ptIndex,
						__global SResponseLayer* responselayers, __read_only image2d_t rBuffer, 
						__read_only image2d_t lBuffer,
						__global SIpointSmall* points)
{
	float derivates[3];
	float hessian[9];
	
	BuildDerivativeImage(r, c, t, m, b, width, height, rBuffer, derivates, responselayers);
	BuildHessianImage(r, c, t, m, b, width, height, rBuffer, responselayers, hessian);
	inverse3(hessian);
	
	multiply3(-1.0f, hessian, derivates);

	// get the step distance between filters
	int filterStep = (responselayers[m].filter - responselayers[b].filter);
	
	// If point is sufficiently close to the actual extremum
	if (fabs(derivates[0]) < 0.5f && fabs(derivates[1]) < 0.5f && fabs(derivates[2]) < 0.5f)
	{
        points[ptIndex].x = (float)((c + derivates[0]) * responselayers[t].step);
        points[ptIndex].y = (float)((r + derivates[1]) * responselayers[t].step);
        points[ptIndex].scale = (float)((0.1333f) * (responselayers[m].filter + derivates[2] * filterStep));
        points[ptIndex].laplacian = getLaplacianImage(r,c,m,t,width, height, responselayers, lBuffer);

		return true;
	}
    
	return false;
}

bool isExtremum(int r, int c, int t, int m, int b, int width, int height, float thresh,
											__global SResponseLayer* responselayers,
											__global float* rBuffer)
{
	// bounds check
	int layerBorder = (responselayers[t].filter + 1) / (2 * responselayers[t].step);
	if (r <= layerBorder || r >= responselayers[t].height - layerBorder || c <= layerBorder || c >= responselayers[t].width - layerBorder)
        return false;
	
	// check the candidate point in the middle layer is above thresh 
	float candidate = getResponse(r, c, t, m, width, height, rBuffer, responselayers);
	if (candidate < thresh)
        return false;
	
	for (int rr = -1; rr <= 1; ++rr)
	{
        for (int cc = -1; cc <= 1; ++cc)
        {
			// if any response in 3x3x3 is greater candidate not maximum
			if (getResponse(r + rr, c + cc, t, -1, width, height, rBuffer, responselayers) >= candidate ||
				((rr != 0 || cc != 0) && getResponse(r + rr, c + cc, t, m, width, height, rBuffer, responselayers) >= candidate) ||
				getResponse(r + rr, c + cc, t, b, width, height, rBuffer, responselayers) >= candidate)
			{
				return false;
			}
        }
	}
	return true;
}

bool isExtremumImage(int r, int c, int t, int m, int b, int width, int height, float thresh,
											__global SResponseLayer* responselayers,
											__read_only image2d_t rBuffer)
{
	// bounds check
	int layerBorder = (responselayers[t].filter + 1) / (2 * responselayers[t].step);
	if (r <= layerBorder || r >= responselayers[t].height - layerBorder || c <= layerBorder || c >= responselayers[t].width - layerBorder)
        return false;
	
	// check the candidate point in the middle layer is above thresh 
	float candidate = getResponseImage(r, c, t, m, width, height, rBuffer, responselayers);
	if (candidate < thresh)
        return false;
	
	for (int rr = -1; rr <= 1; ++rr)
	{
        for (int cc = -1; cc <= 1; ++cc)
        {
			// if any response in 3x3x3 is greater candidate not maximum
			if (getResponseImage(r + rr, c + cc, t, -1, width, height, rBuffer, responselayers) >= candidate ||
				((rr != 0 || cc != 0) && getResponseImage(r + rr, c + cc, t, m, width, height, rBuffer, responselayers) >= candidate) ||
				getResponseImage(r + rr, c + cc, t, b, width, height, rBuffer, responselayers) >= candidate)
			{
				return false;
			}
        }
	}
	return true;
}

float BoxIntegral(int Height, int Width, int row, int col, int rows, int cols, __global float* imageData)
{
	//col x row  ist die linke obere Ecke des "Filterkerns"
	//col + cols x row + rows ist die rechte untere Ecke
    // The subtraction by one for row/col is because row/col is inclusive.
    int r1 = min(row, Height) - 1;
    int c1 = min(col, Width) - 1;
    int r2 = min(row + rows, Height) - 1;
    int c2 = min(col + cols, Width) - 1;

    float A = 0, B = 0, C = 0, D = 0;
    if (r1 >= 0 && c1 >= 0) A = imageData[(int)mad((float)r1, (float)Width, (float)c1)]; //links oben
    if (r1 >= 0 && c2 >= 0) B = imageData[(int)mad((float)r1, (float)Width, (float)c2)]; //rechts oben
    if (r2 >= 0 && c1 >= 0) C = imageData[(int)mad((float)r2, (float)Width, (float)c1)]; //links unten
    if (r2 >= 0 && c2 >= 0) D = imageData[(int)mad((float)r2, (float)Width, (float)c2)]; //rechts unten

    return fmax(0.0f, A - B - C + D);
}

float BoxIntegralImage(int row, int col, int rows, int cols, __read_only image2d_t imageData)
{	
	int Width = get_image_width(imageData);
	int Height = get_image_height(imageData);
	//col x row  ist die linke obere Ecke des "Filterkerns"
	//col + cols x row + rows ist die rechte untere Ecke
    // The subtraction by one for row/col is because row/col is inclusive.
    int r1 = min(row, Height) - 1;
    int c1 = min(col, Width) - 1;
    int r2 = min(row + rows, Height) - 1;
    int c2 = min(col + cols, Width) - 1;

    float A = 0, B = 0, C = 0, D = 0;

	
	//links oben
    if (r1 >= 0 && c1 >= 0)
	{
		//A = imageData[(int)mad((float)r1, (float)Width, (float)c1)]; 
		int2 pos = {c1,r1};
		A = read_imagef(imageData, sampler, pos).x; 
	}
	
	//rechts oben
    if (r1 >= 0 && c2 >= 0)
	{
		//B = imageData[(int)mad((float)r1, (float)Width, (float)c2)];
		int2 pos = {c2, r1};
		B = read_imagef(imageData, sampler, pos).x; 
	}

	//links unten
    if (r2 >= 0 && c1 >= 0)
	{
		//C = imageData[(int)mad((float)r2, (float)Width, (float)c1)]; 
		int2 pos = {c1, r2};
		C = read_imagef(imageData, sampler, pos).x; 
	}
	
	//rechts unten
    if (r2 >= 0 && c2 >= 0) 
	{
		//D = imageData[(int)mad((float)r2, (float)Width, (float)c2)]; 
		int2 pos = {c2, r2};
		D = read_imagef(imageData, sampler, pos).x; 
	}

    return fmax(0.0f, A - B - C + D);
}

/*__kernel void FastHessian(const unsigned int height, 
											const unsigned int width, 
											const float thresh, 
											const int octaves, 
											const int init_sample,
											const int count_responselayers,
											__global unsigned int *iIPoint,
											__global float* imageData,
											__global SIpoint* points,
											__global SResponseLayer* responselayers,
											__global float* rBuffer,
											__global unsigned char* lBuffer)
{
	unsigned int Count = 0;
	//indexies der Responselayer in der Responsemap
	int filter_map[20] = {0,1,2,3,1,3,4,5,3,5,6,7,5,7,8,9,7,9,10,11};
	
    // Responsemaps bauen
    int w = (width / init_sample);
	int h = (height / init_sample);
	int s = init_sample;
	
	//Oktave 1 enthält Layer 0 - 3
	//filter -> FilterCore
	//Bsp.
	//	Layer 0   init_samples = 2  width = 100 height = 100
	//  -> größe des Layers: 50 x 50
	//  -> jeder 2. Pixel wird mit der Filtergröße 9 (  9 x 9 ) ausgelesen
	//Layer 1,2,3 mit unterschiedlichen Filtergrößen (immer ungenauer)
	if (octaves >= 1)
	{
		responselayers[0].width = w; responselayers[0].height = h; responselayers[0].step = s; responselayers[0].filter = 9;
		responselayers[1].width = w; responselayers[1].height = h; responselayers[1].step = s; responselayers[1].filter = 15;
		responselayers[2].width = w; responselayers[2].height = h; responselayers[2].step = s; responselayers[2].filter = 21;
		responselayers[3].width = w; responselayers[3].height = h; responselayers[3].step = s; responselayers[3].filter = 27;
	}
	
	if (octaves >= 2)
	{
		responselayers[4].width = w / 2; responselayers[4].height = h / 2; responselayers[4].step = s * 2; responselayers[4].filter = 39;
		responselayers[5].width = w / 2; responselayers[5].height = h / 2; responselayers[5].step = s * 2; responselayers[5].filter = 51;
	}
	
	if (octaves >= 3)
	{
		responselayers[6].width = w / 4; responselayers[6].height = h / 4; responselayers[6].step = s * 4; responselayers[6].filter = 75;
		responselayers[7].width = w / 4; responselayers[7].height = h / 4; responselayers[7].step = s * 4; responselayers[7].filter = 99;
	}
	
	if (octaves >= 4)
	{
		responselayers[8].width = w / 8; responselayers[8].height = h / 8; responselayers[8].step = s * 8; responselayers[8].filter = 147;
		responselayers[9].width = w / 8; responselayers[9].height = h / 8; responselayers[9].step = s * 8; responselayers[9].filter = 195;
	}
	
	if (octaves >= 5)
	{
		responselayers[10].width = w / 16; responselayers[10].height = h / 16; responselayers[10].step = s * 16; responselayers[10].filter = 291;
		responselayers[11].width = w / 16; responselayers[11].height = h / 16; responselayers[11].step = s * 16; responselayers[11].filter = 387;
	}
	
	// Berechnen ResponseLayer
	for ( int i = 0; i < count_responselayers; i++)
	{
		//Schrittgröße: der Filterkern wird auf jedes stepte Pixel angewendet
		int step = responselayers[i].step;

		//???Abstand des mittleren Pixel zum Rand des Filters
		//z.B. (9-1) / 2 + 1 = 5
		// 0.  1.  2.  3.  4.  5.  6.  7.  8.
		int b = (responselayers[i].filter - 1) / 2 + 1;

		//???Abstand zu den Seiten
		// 9 : 3 = 3 ->  9x9 Filter
		int l = responselayers[i].filter / 3;
		int w = responselayers[i].filter;
		
		//Multiplikator für das Endergebni
		//Bsp: 1 / 81 -> zur Multiplikation mit dem Filterwert eines 9x9 Filters
		//damit der Wert nicht über 255 wird und jeder Filterwert gleichgewichtet ist
		float inverse_area = 1.0f / (w * w);       				// normalisation factor

		//Ableitung 2 mal nach x / y und xy
		float Dxx, Dyy, Dxy;
	
		//index -> index im Ziellayer (ein dimensionales Array)
		//ar und ac sind y und x Koordinate im Ziellayer
		for (int r, c, ar = 0, index = 0; ar < responselayers[i].height; ++ar)
		{
    	    for (int ac = 0; ac < responselayers[i].width; ++ac, index++)
     	   {				
				//r und c sind y und x Koordinate im Integralimage
				r = ar * step;
				c = ac * step;
			
				//Filter auf das Bild anwenden und Filterantworten speichern
				// 2 mal Ableitung in x Richtung
				                                //y Koordinate der linken oberen Ecke
															//x Koordinate
															       
				Dxx = BoxIntegral(height, width, r - l + 1, c - b, 2 * l - 1, w, imageData)
					- BoxIntegral(height, width, r - l + 1, c - l / 2, 2 * l - 1, l, imageData) * 3;
				// 2 mal Ableitung in y Richtung
				Dyy = BoxIntegral(height, width, r - b, c - l + 1, w, 2 * l - 1, imageData)
					- BoxIntegral(height, width, r - l / 2, c - l + 1, l, 2 * l - 1, imageData) * 3;
				// einmal nach x und dann nach y abgeleitet
				Dxy = + BoxIntegral(height, width, r - l, c + 1, l, l, imageData)
					+ BoxIntegral(height, width, r + 1, c - l, l, l, imageData)
					- BoxIntegral(height, width, r - l, c - l, l, l, imageData)
					- BoxIntegral(height, width, r + 1, c + 1, l, l, imageData);
			
				// Normalise the filter responses with respect to their size
				Dxx *= inverse_area;
				Dyy *= inverse_area;
				Dxy *= inverse_area;
			
				// Get the determinant of hessian response & laplacian sign
				rBuffer[i*height*width + index] = (Dxx * Dyy - 0.81f * Dxy * Dxy);
				lBuffer[i*height*width + index] = (Dxx + Dyy >= 0 ? 1 : 0);
			}
		}	
	}

	// für Ipoints
	for (int o = 0; o < octaves; ++o) {
		for (int j = 0; j <= 1; ++j) {
			int b = filter_map[4 * o + j];
			int m = filter_map[4 * o + j + 1];
			int t = filter_map[4 * o + j + 2];
	
			for (int r = 0; r < responselayers[t].height; ++r) {
				for (int c = 0; c < responselayers[t].width; ++c) {
					if (isExtremum(r, c, t, m, b, width, height, thresh, responselayers, rBuffer)) {
						
						if (interpolateExtremum(r, c, t, m, b, width, height, iIPoint, responselayers, rBuffer, lBuffer, points))
						{
							Count++;
						}
					}
				}
			}
		}
	}
	
	//(*iIPoint) = Count;
}*/

/// Versuch der Parallelisierung
__kernel void FastHessianParallel(const unsigned int height, 
											const unsigned int width, 
											const float thresh, 
											const int octaves, 
											const int init_sample,
											const int count_responselayers,
											__global float* imageData,
											__global SIpoint* points,
											__global SResponseLayer* responselayers,
											__global float* rBuffer,
											__global unsigned char* lBuffer)
{	
    // Responsemaps bauen
    if (get_local_id(0) > 0)
    {
    	return;
    }
    
    int w = (width / init_sample);
	int h = (height / init_sample);
	int s = init_sample;
	
	//Oktave 1 enthält Layer 0 - 3
	//filter -> FilterCore
	//Bsp.
	//	Layer 0   init_samples = 2  width = 100 height = 100
	//  -> größe des Layers: 50 x 50
	//  -> jeder 2. Pixel wird mit der Filtergröße 9 (  9 x 9 ) ausgelesen
	//Layer 1,2,3 mit unterschiedlichen Filtergrößen (immer ungenauer)
	if (octaves >= 1)
	{
		responselayers[0].width = w; responselayers[0].height = h; responselayers[0].step = s; responselayers[0].filter = 9;
		responselayers[1].width = w; responselayers[1].height = h; responselayers[1].step = s; responselayers[1].filter = 15;
		responselayers[2].width = w; responselayers[2].height = h; responselayers[2].step = s; responselayers[2].filter = 21;
		responselayers[3].width = w; responselayers[3].height = h; responselayers[3].step = s; responselayers[3].filter = 27;
	}
	
	if (octaves >= 2)
	{
		responselayers[4].width = w / 2; responselayers[4].height = h / 2; responselayers[4].step = s * 2; responselayers[4].filter = 39;
		responselayers[5].width = w / 2; responselayers[5].height = h / 2; responselayers[5].step = s * 2; responselayers[5].filter = 51;
	}
	
	if (octaves >= 3)
	{
		responselayers[6].width = w / 4; responselayers[6].height = h / 4; responselayers[6].step = s * 4; responselayers[6].filter = 75;
		responselayers[7].width = w / 4; responselayers[7].height = h / 4; responselayers[7].step = s * 4; responselayers[7].filter = 99;
	}
	
	if (octaves >= 4)
	{
		responselayers[8].width = w / 8; responselayers[8].height = h / 8; responselayers[8].step = s * 8; responselayers[8].filter = 147;
		responselayers[9].width = w / 8; responselayers[9].height = h / 8; responselayers[9].step = s * 8; responselayers[9].filter = 195;
	}
	
	if (octaves >= 5)
	{
		responselayers[10].width = w / 16; responselayers[10].height = h / 16; responselayers[10].step = s * 16; responselayers[10].filter = 291;
		responselayers[11].width = w / 16; responselayers[11].height = h / 16; responselayers[11].step = s * 16; responselayers[11].filter = 387;
	}
}

__kernel void ResponseLayerParallel(const unsigned int height, 
											const unsigned int width, 
											const float thresh, 
											const int octaves, 
											const int init_sample,
											const int count_responselayers,
											__global float* imageData,
											__global SIpoint* points,
											__global SResponseLayer* responselayers,
											__global float* rBuffer,
											__global unsigned char* lBuffer)
{
	// Berechnen ResponseLayer
	size_t id =  get_local_id(0);
	//Bei hohen Bildern muss ein waagerecht arbeitendes Workitem mehrere Zeilen bearbeiten
	size_t linesPerItem = ceil(count_responselayers / (float)get_local_size(0));
	
	for(int i = id*linesPerItem; i < count_responselayers; i+= get_global_size(0))
	{
		//Schrittgröße: der Filterkern wird auf jedes stepte Pixel angewendet
		int step = responselayers[i].step;

		//???Abstand des mittleren Pixel zum Rand des Filters
		//z.B. (9-1) / 2 + 1 = 5
		// 0.  1.  2.  3.  4.  5.  6.  7.  8.
		int b = (responselayers[i].filter - 1) / 2 + 1;

		//???Abstand zu den Seiten
		// 9 : 3 = 3 ->  9x9 Filter
		int l = responselayers[i].filter / 3;
		int w = responselayers[i].filter;
		
		//Multiplikator für das Endergebni
		//Bsp: 1 / 81 -> zur Multiplikation mit dem Filterwert eines 9x9 Filters
		//damit der Wert nicht über 255 wird und jeder Filterwert gleichgewichtet ist
		float inverse_area = 1.0f / (w * w);       				// normalisation factor

		//Ableitung 2 mal nach x / y und xy
		float Dxx, Dyy, Dxy;
	
		//index -> index im Ziellayer (ein dimensionales Array)
		//ar und ac sind y und x Koordinate im Ziellayer
		for (int r, c, ar = 0, index = 0; ar < responselayers[i].height; ++ar)
		{
    	    for (int ac = 0; ac < responselayers[i].width; ++ac, index++)
     	   {				
				//r und c sind y und x Koordinate im Integralimage
				r = ar * step;
				c = ac * step;
			
				//Filter auf das Bild anwenden und Filterantworten speichern
				// 2 mal Ableitung in x Richtung
				                                //y Koordinate der linken oberen Ecke
															//x Koordinate
															       
				Dxx = BoxIntegral(height, width, r - l + 1, c - b, 2 * l - 1, w, imageData)
					- BoxIntegral(height, width, r - l + 1, c - l / 2, 2 * l - 1, l, imageData) * 3;
				// 2 mal Ableitung in y Richtung
				Dyy = BoxIntegral(height, width, r - b, c - l + 1, w, 2 * l - 1, imageData)
					- BoxIntegral(height, width, r - l / 2, c - l + 1, l, 2 * l - 1, imageData) * 3;
				// einmal nach x und dann nach y abgeleitet
				Dxy = + BoxIntegral(height, width, r - l, c + 1, l, l, imageData)
					+ BoxIntegral(height, width, r + 1, c - l, l, l, imageData)
					- BoxIntegral(height, width, r - l, c - l, l, l, imageData)
					- BoxIntegral(height, width, r + 1, c + 1, l, l, imageData);
			
				// Normalise the filter responses with respect to their size
				Dxx *= inverse_area;
				Dyy *= inverse_area;
				Dxy *= inverse_area;
			
				// Get the determinant of hessian response & laplacian sign
				rBuffer[i*height*width + index] = (Dxx * Dyy - 0.81f * Dxy * Dxy);
				lBuffer[i*height*width + index] = (Dxx + Dyy >= 0 ? 1 : 0);
			}
		}
	}
}
// Berechnen ResponseLayer. IntegralImage als image2d_t
__kernel void ResponseLayerParallelV3(		__read_only image2d_t imageData,
											const unsigned int countresponselayers,
											__global SResponseLayer* responselayers,
											__global float* rBuffer,
											__global unsigned char* lBuffer,
											__local int* step,
											__local int* b,
											__local int* l,
											__local int* w,
											__local float* inverse_area)
{
	// Initialisierung der lokalen Buffer
	int MaxWidth = 0;
	int MaxHeight = 0;
	int width = get_image_width(imageData);
	int height = get_image_height(imageData);

	for ( int i = 0; i < countresponselayers; i++)
	{
		step[i] = responselayers[i].step;
		
		b[i] = (responselayers[i].filter - 1) / 2 + 1;
		
		l[i] = responselayers[i].filter / 3;
		w[i] = responselayers[i].filter;
		
		inverse_area[i] = 1.0f / (w[i] * w[i]);  
		
		if (MaxHeight < responselayers[i].height)
		{
			MaxHeight = responselayers[i].height;
		}
		
		if (MaxWidth < responselayers[i].width)
		{
			MaxWidth = responselayers[i].width;
		}
	}
	
	// id von 0 .. responselayer[i].height
	size_t id =  get_local_id(0);
	
	//Ableitung 2 mal nach x / y und xy
	float Dxx, Dyy, Dxy;
	
	// index speichern
	int index = 0;
	int localindex = 0;
	
	
	for(int r, c, ar = id; ar < MaxHeight; ar+= get_global_size(0))
	{
		for (int ac = 0; ac < MaxWidth; ++ac)
		{
			for (int i = countresponselayers - 1; i >= 0; i--)
			{
				if ((ar < responselayers[i].height) && (ac < responselayers[i].width))
				{
					index = i*height*width;
					
					//r und c sind y und x Koordinate im Integralimage
					r = ar * step[i];
					c = ac * step[i];
			
					//Filter auf das Bild anwenden und Filterantworten speichern
					// 2 mal Ableitung in x Richtung
				                                //y Koordinate der linken oberen Ecke
															//x Koordinate
				

					Dxx = BoxIntegralImage(r - l[i] + 1, c - b[i], 2 * l[i] - 1, w[i], imageData)
						- BoxIntegralImage(r - l[i] + 1, c - l[i] / 2, 2 * l[i] - 1, l[i], imageData) * 3;
					// 2 mal Ableitung in y Richtung
					Dyy = BoxIntegralImage(r - b[i], c - l[i] + 1, w[i], 2 * l[i] - 1, imageData)
						- BoxIntegralImage(r - l[i] / 2, c - l[i] + 1, l[i], 2 * l[i] - 1, imageData) * 3;
					// einmal nach x und dann nach y abgeleitet
					Dxy = + BoxIntegralImage(r - l[i], c + 1, l[i], l[i], imageData)
						+ BoxIntegralImage(r + 1, c - l[i], l[i], l[i], imageData)
						- BoxIntegralImage(r - l[i], c - l[i], l[i], l[i], imageData)
						- BoxIntegralImage(r + 1, c + 1, l[i], l[i], imageData);
			
					// Normalise the filter responses with respect to their size
					Dxx *= inverse_area[i];
					Dyy *= inverse_area[i];
					Dxy *= inverse_area[i];
			
					// Get the determinant of hessian response & laplacian sign
					localindex = index + (int)mad((float)responselayers[i].width, (float)ar, (float)ac);
					rBuffer[localindex] = (mad((float)Dxx, (float)Dyy, (float)- 0.81f * Dxy * Dxy));
					lBuffer[localindex] = (Dxx + Dyy >= 0 ? 1 : 0);
				}
			}
		}
			//index += (get_global_size(0)-1)*responselayers[i].width;
	}
}

// Berechnen ResponseLayer. IntegralImage als image2d_t, Responsebuffer als image
__kernel void ResponseLayerParallelV4(		__read_only image2d_t imageData,
											const unsigned int countresponselayers,
											__global SResponseLayer* responselayers,
											__write_only image2d_t rBuffer,
											__write_only image2d_t lBuffer,
											__local int* step,
											__local int* b,
											__local int* l,
											__local int* w,
											__local float* inverse_area)
{
	// Initialisierung der lokalen Buffer
	int MaxWidth = 0;
	int MaxHeight = 0;
	int width = get_image_width(imageData);
	int height = get_image_height(imageData);

	for ( int i = 0; i < countresponselayers; i++)
	{
		step[i] = responselayers[i].step;
		
		b[i] = (responselayers[i].filter - 1) / 2 + 1;
		
		l[i] = responselayers[i].filter / 3;
		w[i] = responselayers[i].filter;
		
		inverse_area[i] = 1.0f / (w[i] * w[i]);  
		
		if (MaxHeight < responselayers[i].height)
		{
			MaxHeight = responselayers[i].height;
		}
		
		if (MaxWidth < responselayers[i].width)
		{
			MaxWidth = responselayers[i].width;
		}
	}
	
	// id von 0 .. responselayer[i].height
	size_t id =  get_local_id(0);
	
	//Ableitung 2 mal nach x / y und xy
	float Dxx, Dyy, Dxy;
	
	for(int r, c, ar = id; ar < MaxHeight; ar+= get_global_size(0))
	{
		for (int ac = 0; ac < MaxWidth; ++ac)
		{
			for (int i = countresponselayers - 1; i >= 0; i--)
			{
				if ((ar < responselayers[i].height) && (ac < responselayers[i].width))
				{
					//r und c sind y und x Koordinate im Integralimage
					r = ar * step[i];
					c = ac * step[i];
			
					//Filter auf das Bild anwenden und Filterantworten speichern
					// 2 mal Ableitung in x Richtung
				                                //y Koordinate der linken oberen Ecke
															//x Koordinate
				

					Dxx = BoxIntegralImage(r - l[i] + 1, c - b[i], 2 * l[i] - 1, w[i], imageData)
						- BoxIntegralImage(r - l[i] + 1, c - l[i] / 2, 2 * l[i] - 1, l[i], imageData) * 3;
					// 2 mal Ableitung in y Richtung
					Dyy = BoxIntegralImage(r - b[i], c - l[i] + 1, w[i], 2 * l[i] - 1, imageData)
						- BoxIntegralImage(r - l[i] / 2, c - l[i] + 1, l[i], 2 * l[i] - 1, imageData) * 3;
					// einmal nach x und dann nach y abgeleitet
					Dxy = + BoxIntegralImage(r - l[i], c + 1, l[i], l[i], imageData)
						+ BoxIntegralImage(r + 1, c - l[i], l[i], l[i], imageData)
						- BoxIntegralImage(r - l[i], c - l[i], l[i], l[i], imageData)
						- BoxIntegralImage(r + 1, c + 1, l[i], l[i], imageData);
			
					// Normalise the filter responses with respect to their size
					Dxx *= inverse_area[i];
					Dyy *= inverse_area[i];
					Dxy *= inverse_area[i];
			
					// Get the determinant of hessian response & laplacian sign
					int2 position = {ac,i*height + ar};
					write_imagef(rBuffer, position, mad((float)Dxx, (float)Dyy, (float)- 0.81f * Dxy * Dxy));
					write_imageui(lBuffer, position, (Dxx + Dyy >= 0 ? 1 : 0));
				}
			}
		}
			//index += (get_global_size(0)-1)*responselayers[i].width;
	}
}
// Berechnen ResponseLayer
__kernel void ResponseLayerParallelV2(const unsigned int height, 
											const unsigned int width, 
											const unsigned int countresponselayers,
											__global float* imageData,
											__global SResponseLayer* responselayers,
											__global float* rBuffer,
											__global unsigned char* lBuffer,
											__local int* step,
											__local int* b,
											__local int* l,
											__local int* w,
											__local float* inverse_area)
{
	// Initialisierung der lokalen Buffer
	int MaxWidth = 0;
	int MaxHeight = 0;
	
	for ( int i = 0; i < countresponselayers; i++)
	{
		step[i] = responselayers[i].step;
		
		b[i] = (responselayers[i].filter - 1) / 2 + 1;
		
		l[i] = responselayers[i].filter / 3;
		w[i] = responselayers[i].filter;
		
		inverse_area[i] = 1.0f / (w[i] * w[i]);  
		
		if (MaxHeight < responselayers[i].height)
		{
			MaxHeight = responselayers[i].height;
		}
		
		if (MaxWidth < responselayers[i].width)
		{
			MaxWidth = responselayers[i].width;
		}
	}
	
	// id von 0 .. responselayer[i].height
	size_t id =  get_local_id(0);
	
	//Ableitung 2 mal nach x / y und xy
	float Dxx, Dyy, Dxy;
	
	// index speichern
	int index = 0;
	int localindex = 0;
	

	for(int r, c, ar = id; ar < MaxHeight; ar+= get_global_size(0))
	{
		for (int ac = 0; ac < MaxWidth; ++ac)
		{
			for (int i = countresponselayers - 1; i >= 0; i--)
			{
				if ((ar < responselayers[i].height) && (ac < responselayers[i].width))
				{
					index = i*height*width;
					
					//r und c sind y und x Koordinate im Integralimage
					r = ar * step[i];
					c = ac * step[i];
			
					//Filter auf das Bild anwenden und Filterantworten speichern
					// 2 mal Ableitung in x Richtung
				                                //y Koordinate der linken oberen Ecke
															//x Koordinate
				

					Dxx = BoxIntegral(height, width, r - l[i] + 1, c - b[i], 2 * l[i] - 1, w[i], imageData)
						- BoxIntegral(height, width, r - l[i] + 1, c - l[i] / 2, 2 * l[i] - 1, l[i], imageData) * 3;
					// 2 mal Ableitung in y Richtung
					Dyy = BoxIntegral(height, width, r - b[i], c - l[i] + 1, w[i], 2 * l[i] - 1, imageData)
						- BoxIntegral(height, width, r - l[i] / 2, c - l[i] + 1, l[i], 2 * l[i] - 1, imageData) * 3;
					// einmal nach x und dann nach y abgeleitet
					Dxy = + BoxIntegral(height, width, r - l[i], c + 1, l[i], l[i], imageData)
						+ BoxIntegral(height, width, r + 1, c - l[i], l[i], l[i], imageData)
						- BoxIntegral(height, width, r - l[i], c - l[i], l[i], l[i], imageData)
						- BoxIntegral(height, width, r + 1, c + 1, l[i], l[i], imageData);
			
					// Normalise the filter responses with respect to their size
					Dxx *= inverse_area[i];
					Dyy *= inverse_area[i];
					Dxy *= inverse_area[i];
			
					// Get the determinant of hessian response & laplacian sign
					localindex = index + (int)mad((float)responselayers[i].width, (float)ar, (float)ac);
					rBuffer[localindex] = (mad((float)Dxx, (float)Dyy, (float)- 0.81f * Dxy * Dxy));
					lBuffer[localindex] = (Dxx + Dyy >= 0 ? 1 : 0);
				}
			}
		}
			//index += (get_global_size(0)-1)*responselayers[i].width;
	}
}
/*
__kernel void ResponseLayerParallelV2(const unsigned int height, 
											const unsigned int width, 
											const unsigned int i,
											__global float* imageData,
											__global SResponseLayer* responselayers,
											__global float* rBuffer,
											__global unsigned char* lBuffer)
{
	// id von 0 .. responselayer[i].height
	size_t id =  get_local_id(0);
	
	// bei höheren Bildern als wir Workitems haben, muss ein Item mehrere Höhen abarbeiten
	size_t linesPerItem = ceil(responselayers[i].height / (float)get_local_size(0));
	
	//Ableitung 2 mal nach x / y und xy
	float Dxx, Dyy, Dxy;
	
	//Schrittgröße: der Filterkern wird auf jedes stepte Pixel angewendet
	int step = responselayers[i].step;
		
	//???Abstand des mittleren Pixel zum Rand des Filters
	//z.B. (9-1) / 2 + 1 = 5
	// 0.  1.  2.  3.  4.  5.  6.  7.  8.
	int b = (responselayers[i].filter - 1) / 2 + 1;
	
	//???Abstand zu den Seiten
	// 9 : 3 = 3 ->  9x9 Filter
	int l = responselayers[i].filter / 3;
	int w = responselayers[i].filter;
	
	//Multiplikator für das Endergebni
	//Bsp: 1 / 81 -> zur Multiplikation mit dem Filterwert eines 9x9 Filters
	//damit der Wert nicht über 255 wird und jeder Filterwert gleichgewichtet ist
	float inverse_area = 1.0f / (w * w);       				// normalisation factor
	
	// index speichern
	int index = i*height*width;
	int localindex = 0;
	
	for(int r, c, ar = id; ar < responselayers[i].height; ar+= get_global_size(0))
	{
			for (int ac = 0; ac < responselayers[i].width; ++ac)
			{
				//r und c sind y und x Koordinate im Integralimage
				r = ar * step;
				c = ac * step;
			
				//Filter auf das Bild anwenden und Filterantworten speichern
				// 2 mal Ableitung in x Richtung
				                                //y Koordinate der linken oberen Ecke
															//x Koordinate
				

				Dxx = BoxIntegral(height, width, r - l + 1, c - b, 2 * l - 1, w, imageData)
					- BoxIntegral(height, width, r - l + 1, c - l / 2, 2 * l - 1, l, imageData) * 3;
				// 2 mal Ableitung in y Richtung
				Dyy = BoxIntegral(height, width, r - b, c - l + 1, w, 2 * l - 1, imageData)
					- BoxIntegral(height, width, r - l / 2, c - l + 1, l, 2 * l - 1, imageData) * 3;
				// einmal nach x und dann nach y abgeleitet
				Dxy = + BoxIntegral(height, width, r - l, c + 1, l, l, imageData)
					+ BoxIntegral(height, width, r + 1, c - l, l, l, imageData)
					- BoxIntegral(height, width, r - l, c - l, l, l, imageData)
					- BoxIntegral(height, width, r + 1, c + 1, l, l, imageData);
			
				// Normalise the filter responses with respect to their size
				Dxx *= inverse_area;
				Dyy *= inverse_area;
				Dxy *= inverse_area;
			
				// Get the determinant of hessian response & laplacian sign
				localindex = index + (int)mad((float)responselayers[i].width, ar, ac);
				rBuffer[localindex] = (mad((float)Dxx, Dyy, - 0.81f * Dxy * Dxy));
				lBuffer[localindex] = (Dxx + Dyy >= 0 ? 1 : 0);
			}
			//index += (get_global_size(0)-1)*responselayers[i].width;
	}
}*/
/*__kernel void IPointsParallel(const unsigned int height, 
											const unsigned int width, 
											const float thresh, 
											const int octaves, 
											const int init_sample,
											const int count_responselayers,
											__global float* imageData,
											__global SIpoint* points,
											__global SResponseLayer* responselayers,
											__global float* rBuffer,
											__global unsigned char* lBuffer)
{
	//indexies der Responselayer in der Responsemap
	int filter_map[20] = {0,1,2,3,1,3,4,5,3,5,6,7,5,7,8,9,7,9,10,11};
	
	// für Ipoints
	int iIpoint = 0;
	
	for (int o = 0; o < octaves; ++o) {
		for (int j = 0; j <= 1; ++j) {
			int b = filter_map[4 * o + j];
			int m = filter_map[4 * o + j + 1];
			int t = filter_map[4 * o + j + 2];
	
			for (int r = 0; r < responselayers[t].height; ++r) {
				for (int c = 0; c < responselayers[t].width; ++c) {
					if (isExtremum(r, c, t, m, b, width, height, thresh, responselayers, rBuffer)) {
						if (interpolateExtremum(r, c, t, m, b, width, height, iIpoint, responselayers, rBuffer, lBuffer, points))
						{
							iIpoint++;
						}
					}
				}
			}
		}
	}
}*/

/* Alte version die über die höhe geht
__kernel void IPointsParallelV2(
											const unsigned int t,
											const unsigned int m,
											const unsigned int b,
											const unsigned int width, 
											const unsigned int height,
											const float thresh, 
											__global unsigned int* iIpoint,
											__global SIpoint* points,
											__global SResponseLayer* responselayers,
											__global float* rBuffer,
											__global unsigned char* lBuffer)
{
	size_t id =  get_local_id(0);
	iIpoint[id] = 0;
	//Bei hohen Bildern muss ein waagerecht arbeitendes Workitem mehrere Zeilen bearbeiten
	size_t linesPerItem = ceil(height / (float)get_local_size(0));
	
	iIpoint[id] = 0;
		
	for(int r = id*linesPerItem; r < height; r+= get_global_size(0))
	{
		for (int c = 0; c < responselayers[t].width; ++c) {
			if (isExtremum(r, c, t, m, b, width, height, thresh, responselayers, rBuffer)) {
				if (interpolateExtremum(r, c, t, m, b, width, height, r*width + iIpoint[id], responselayers, rBuffer, lBuffer, points))
				{
					iIpoint[id]++;
				}
			}
		}
	}
}*/

__kernel void IPointsParallelV2(
											const unsigned int t,
											const unsigned int m,
											const unsigned int b,
											const unsigned int width, 
											const unsigned int height,
											const float thresh, 
											__global unsigned int* iIpoint,
											__global SIpointSmall* points,
											__global SResponseLayer* responselayers,
											__global float* rBuffer,
											__global unsigned char* lBuffer)
{

	// Daten lokal kopieren
	size_t id =  get_local_id(0);
	int index = 0;
	
	for(int c = id; c < width; c+= get_global_size(0))
	{
		index = c * height;
		for (int r = 0; r < responselayers[t].height; ++r) {
			if (isExtremum(r, c, t, m, b, width, height, thresh, responselayers, rBuffer)) {
				if (interpolateExtremum(r, c, t, m, b, width, height, index + iIpoint[c], responselayers, rBuffer, lBuffer, points))
				{
					iIpoint[c]++;
				}
			}
		}
	}
}

__kernel void IPointsParallelV2Image(
											const unsigned int t,
											const unsigned int m,
											const unsigned int b,
											const unsigned int width, 
											const unsigned int height,
											const float thresh, 
											__global unsigned int* iIpoint,
											__global SIpointSmall* points,
											__global SResponseLayer* responselayers,
											__read_only image2d_t rBuffer,
											__read_only image2d_t lBuffer)
{

	// Daten lokal kopieren
	size_t id =  get_local_id(0);
	int index = 0;
	
	for(int c = id; c < width; c+= get_global_size(0))
	{
		index = c * height;
		for (int r = 0; r < responselayers[t].height; ++r) {
			if (isExtremumImage(r, c, t, m, b, width, height, thresh, responselayers, rBuffer)) {
				if (interpolateExtremumImage(r, c, t, m, b, width, height, index + iIpoint[c], responselayers, rBuffer, lBuffer, points))
				{
					iIpoint[c]++;
				}
			}
		}
	}
}


__kernel void GetIPoints(					const unsigned int ptCount,
											const unsigned int workCount,
											const unsigned int width, 
											const unsigned int height,
											__global int* ptsCount,
											__global SIpointSmall* points,
											__global SIpoint* points_out)
{	
	if (get_local_id(0) > 0) return;
	
	int a = 0;
	int offset = 0;
	
	for (int i = 0; i < workCount; i++) {
        if (ptsCount[i] > 0)
		{
			offset = i * height;
			for (int x = 0; x < ptsCount[i]; x++)
			{
				points_out[a].x = points[offset + x].x;
				points_out[a].y = points[offset + x].y;
				points_out[a].scale = points[offset + x].scale;
				points_out[a].laplacian = points[offset + x].laplacian;
				//points_out[a].descriptors = 0;
				a++;
			}
		}
    }
}

__kernel void CalcPointCount(				const unsigned int WorkItems,
											__global unsigned int* iIpoint,
											__global unsigned int* iPtCount)
{	
	*iPtCount = 0;
	for (int i = 0; i < WorkItems; i++) {
        *iPtCount += iIpoint[i];
    }
}
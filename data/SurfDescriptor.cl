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

__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

__constant float pi = 3.14159f;

__constant bool bDes = true;

// Gauss core 25
__constant float gauss25 [7][7] = {
    0.02350693969273,0.01849121369071,0.01239503121241,0.00708015417522,0.00344628101733,0.00142945847484,0.00050524879060,
    0.02169964028389,0.01706954162243,0.01144205592615,0.00653580605408,0.00318131834134,0.00131955648461,0.00046640341759,
    0.01706954162243,0.01342737701584,0.00900063997939,0.00514124713667,0.00250251364222,0.00103799989504,0.00036688592278,
    0.01144205592615,0.00900063997939,0.00603330940534,0.00344628101733,0.00167748505986,0.00069579213743,0.00024593098864,
    0.00653580605408,0.00514124713667,0.00344628101733,0.00196854695367,0.00095819467066,0.00039744277546,0.00014047800980,
    0.00318131834134,0.00250251364222,0.00167748505986,0.00095819467066,0.00046640341759,0.00019345616757,0.00006837798818,
    0.00131955648461,0.00103799989504,0.00069579213743,0.00039744277546,0.00019345616757,0.00008024231247,0.00002836202103
};

// apply gaussian
float gaussian(float x, float y, float sig)
{
    return 1.0f/(2.0f*pi*sig*sig) * exp( -(x*x+y*y)/(2.0f*sig*sig));
}

// Get the anggle for point
float getAngle(float X, float Y)
{
    if(X > 0 && Y >= 0)
        return atan(Y/X);

    if(X < 0 && Y >= 0)
        return pi - atan(-Y/X);

    if(X < 0 && Y < 0)
        return pi + atan(Y/X);

    if(X > 0 && Y < 0)
        return 2*pi - atan(-Y/X);

    return 0;
}

// calculate BoxIntegral for filter
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

//calculate BoxIntegral for filter with image
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
        int2 pos = {c1,r1};
        A = read_imagef(imageData, sampler, pos).x; 
    }

    //rechts oben
    if (r1 >= 0 && c2 >= 0)
    {
        int2 pos = {c2, r1};
        B = read_imagef(imageData, sampler, pos).x; 
    }

    //links unten
    if (r2 >= 0 && c1 >= 0)
    {
        int2 pos = {c1, r2};
        C = read_imagef(imageData, sampler, pos).x; 
    }

    //rechts unten
    if (r2 >= 0 && c2 >= 0) 
    {
        int2 pos = {c2, r2};
        D = read_imagef(imageData, sampler, pos).x; 
    }

    return fmax(0.0f, A - B - C + D);
}

// Haar-functions for x and y
float haarX(int Height, int Width, int row, int column, int size, __global float* imageData)
{
    return BoxIntegral(Height, Width, row - size / 2, column, size, size / 2, imageData)
        - 1 * BoxIntegral(Height, Width, row - size / 2, column - size / 2, size, size / 2, imageData);
}

float haarY(int Height, int Width, int row, int column, int size, __global float* imageData)
{
    return BoxIntegral(Height, Width, row, column - size / 2, size / 2, size, imageData)
        - 1 * BoxIntegral(Height, Width, row - size / 2, column - size / 2, size / 2, size, imageData);
}

// Haar-functions for x and y but with image
float haarXImage(int row, int column, int size, __read_only image2d_t imageData)
{
    return BoxIntegralImage( row - size / 2, column, size, size / 2, imageData)
        - 1 * BoxIntegralImage(row - size / 2, column - size / 2, size, size / 2, imageData);
}

float haarYImage(int row, int column, int size, __read_only image2d_t imageData)
{
    return BoxIntegralImage(row, column - size / 2, size / 2, size, imageData)
        - 1 * BoxIntegralImage(row - size / 2, column - size / 2, size / 2, size, imageData);
}

// Roundfuncation for float our values war just plus but for safity it also handels minus
int fRound(float _Value)
{
    if (_Value < 0)
    {
        return (int)(_Value - 0.5);
    } else {
        return (int)(_Value + 0.5);
    }
}

// our parallelized fuction for the descriptors 
__kernel void SurfDescriptorParallel(	
    const unsigned int height, 
    const unsigned int width,
    const unsigned int IpointCount,
    const unsigned int DescriptorLength,
    __global float* imageData,
    __global SIpoint* points)
{
    // vars for orientation detection
    int y, x, sample_x, sample_y, count=0;
    int ix = 0, j = 0, jx = 0, xs = 0, ys = 0;
    float dx, dy, mdx, mdy, co, si;
    float gauss_s1 = 0.f, gauss_s2 = 0.f;
    float rx = 0.f, ry = 0.f, rrx = 0.f, rry = 0.f, len = 0.f;
    float cx = -0.5f, cy = 0.f; //Subregion centers for the 4x4 gaussian weighting


    // Main loop assigns orientations and gets descriptors
    for (int i = get_local_id(0); i < IpointCount; i += get_global_size(0))
    {
        // Set the Ipoint to be described
        //index = i;

        // Assign Orientations and extract rotation invariant descriptors
        float gauss = 0.f;
        float scale = points[i].scale;
        const int s = fRound(scale), r = fRound(points[i].y), c = fRound(points[i].x);
        float resX[109];
        float resY[109];
        float Ang[109];

        const int id[] = {6,5,4,3,2,1,0,1,2,3,4,5,6};

        int idx = 0;
        // calculate haar responses for points within radius of 6*scale
        for(int x = -6; x <= 6; ++x) 
        {
            for(int j = -6; j <= 6; ++j) 
            {
                if(x*x + j*j < 36) 
                {
                    gauss = (float)(gauss25[id[x+6]][id[j+6]]);
                    resX[idx] = gauss * haarX(height, width, r+j*s, c+x*s, 4*s, imageData);
                    resY[idx] = gauss * haarY(height, width, r+j*s, c+x*s, 4*s, imageData);
                    Ang[idx] = getAngle(resX[idx], resY[idx]);
                    ++idx;
                }
            }
        }

        // calculate the dominant direction 
        float sumX=0.f, sumY=0.f;
        float max=0.f, orientation = 0.f;
        float ang1=0.f, ang2=0.f;

        // loop slides pi/3 window around feature point
        for(ang1 = 0; ang1 < 2*pi;  ang1+=0.15f) {
            ang2 = ( ang1+pi/3.0f > 2*pi ? ang1-5.0f*pi/3.0f : ang1+pi/3.0f);
            sumX = sumY = 0.f; 
            for(unsigned int k = 0; k < 109; ++k) 
            {
                // determine whether the point is within the window
                if (ang1 < ang2 && ang1 < Ang[k] && Ang[k] < ang2) 
                {
                    sumX+=resX[k];  
                    sumY+=resY[k];
                } 
                else if (ang2 < ang1 && 
                    ((Ang[k] > 0 && Ang[k] < ang2) || (Ang[k] > ang1 && Ang[k] < 2*pi) )) 
                {
                    sumX+=resX[k];  
                    sumY+=resY[k];
                }
            }

            // if the vector produced from this window is longer than all 
            // previous vectors then this forms the new dominant direction
            if (sumX*sumX + sumY*sumY > max) 
            {
                // store largest orientation
                max = sumX*sumX + sumY*sumY;
                orientation = getAngle(sumX, sumY);
            }
        }

        // assign orientation of the dominant response vector
        points[i].orientation = orientation;


        /* This needs a huge amount of time and for the most cases and especially our Demo Application it is not used.
        The Descriptor Values can be written to a file and used for later matching, but this is not mentioned in the paper */
        if(bDes)
        {
            // prepare the orientaton detection of the descriptors (64 or 128 for each iPoint)
            scale = points[i].scale;
            x = fRound(points[i].x);
            y = fRound(points[i].y);  
            co = cos(points[i].orientation);
            si = sin(points[i].orientation);

            int a = -8;

            // Calculate descriptor for this interest point (64 or 128 for each iPoint)
            while(a < 12)
            {
                j = -8;
                a = a-4;

                cx += 1.f;
                cy = -0.5f;

                while(j < 12) 
                {
                    dx=dy=mdx=mdy=0.f;
                    cy += 1.f;

                    j = j - 4;

                    ix = a + 5;
                    jx = j + 5;

                    xs = fRound(x + ( -jx*scale*si + ix*scale*co));
                    ys = fRound(y + ( jx*scale*co + ix*scale*si));

                    for (int k = a; k < a + 9; ++k) 
                    {
                        for (int l = j; l < j + 9; ++l) 
                        {
                            //Get coords of sample point on the rotated axis
                            sample_x = fRound(x + (-l*scale*si + k*scale*co));
                            sample_y = fRound(y + ( l*scale*co + k*scale*si));

                            //Get the gaussian weighted x and y responses
                            gauss_s1 = gaussian(xs-sample_x,ys-sample_y,2.5f*scale);
                            rx = haarX(height, width, sample_y, sample_x, 2*fRound(scale), imageData);
                            ry = haarY(height, width, sample_y, sample_x, 2*fRound(scale), imageData);

                            //Get the gaussian weighted x and y responses on rotated axis
                            rrx = gauss_s1*(-rx*si + ry*co);
                            rry = gauss_s1*(rx*co + ry*si);

                            dx += rrx;
                            dy += rry;
                            mdx += fabs(rrx);
                            mdy += fabs(rry);

                        }
                    }

                    //Add the values to the descriptor array via the poiter vector
                    gauss_s2 = gaussian(cx-2.0f,cy-2.0f,1.5f);

                    points[i].descriptors[count++] = dx*gauss_s2;
                    points[i].descriptors[count++] = dy*gauss_s2;
                    points[i].descriptors[count++] = mdx*gauss_s2;
                    points[i].descriptors[count++] = mdy*gauss_s2;

                    // set new len
                    len += (dx*dx + dy*dy + mdx*mdx + mdy*mdy) * gauss_s2*gauss_s2;

                    j += 9;
                }
                a += 9;
            }
            count = 0;

            //Convert to Unit Vector
            len = sqrt(len);
            for(int a = 0; a < DescriptorLength; ++a)
            {
                points[i].descriptors[a] /= len;
            }
        }
    }
}

// Mit IntegralImage als image2d_t mit dem die grafikkarte schneller arbeiten kann
__kernel void SurfDescriptorParallelImage(	
    __read_only image2d_t imageData,
    const unsigned int height, 
    const unsigned int width,
    const unsigned int IpointCount,
    const unsigned int DescriptorLength,
    __global SIpoint* points)
{
    // vars for orientation detection
    int y, x, sample_x, sample_y, count=0;
    int ix = 0, j = 0, jx = 0, xs = 0, ys = 0;
    float dx, dy, mdx, mdy, co, si;
    float gauss_s1 = 0.f, gauss_s2 = 0.f;
    float rx = 0.f, ry = 0.f, rrx = 0.f, rry = 0.f, len = 0.f;
    float cx = -0.5f, cy = 0.f; //Subregion centers for the 4x4 gaussian weighting


    // Main loop assigns orientations and gets descriptors
    for (int i = get_local_id(0); i < IpointCount; i += get_global_size(0))
    {
        // Set the Ipoint to be described
        //index = i;

        // Assign Orientations and extract rotation invariant descriptors
        float gauss = 0.f;
        float scale = points[i].scale;
        const int s = fRound(scale), r = fRound(points[i].y), c = fRound(points[i].x);
        float resX[109];
        float resY[109];
        float Ang[109];

        const int id[] = {6,5,4,3,2,1,0,1,2,3,4,5,6};

        int idx = 0;
        // calculate haar responses for points within radius of 6*scale
        for(int x = -6; x <= 6; ++x) 
        {
            for(int j = -6; j <= 6; ++j) 
            {
                if(x*x + j*j < 36) 
                {
                    gauss = (float)(gauss25[id[x+6]][id[j+6]]);
                    resX[idx] = gauss * haarXImage(r+j*s, c+x*s, 4*s, imageData);
                    resY[idx] = gauss * haarYImage(r+j*s, c+x*s, 4*s, imageData);
                    Ang[idx] = getAngle(resX[idx], resY[idx]);
                    ++idx;
                }
            }
        }

        // calculate the dominant direction 
        float sumX=0.f, sumY=0.f;
        float max=0.f, orientation = 0.f;
        float ang1=0.f, ang2=0.f;

        // loop slides pi/3 window around feature point
        for(ang1 = 0; ang1 < 2*pi;  ang1+=0.15f) {
            ang2 = ( ang1+pi/3.0f > 2*pi ? ang1-5.0f*pi/3.0f : ang1+pi/3.0f);
            sumX = sumY = 0.f; 
            for(unsigned int k = 0; k < 109; ++k) 
            {
                // determine whether the point is within the window
                if (ang1 < ang2 && ang1 < Ang[k] && Ang[k] < ang2) 
                {
                    sumX+=resX[k];  
                    sumY+=resY[k];
                } 
                else if (ang2 < ang1 && 
                    ((Ang[k] > 0 && Ang[k] < ang2) || (Ang[k] > ang1 && Ang[k] < 2*pi) )) 
                {
                    sumX+=resX[k];  
                    sumY+=resY[k];
                }
            }

            // if the vector produced from this window is longer than all 
            // previous vectors then this forms the new dominant direction
            if (sumX*sumX + sumY*sumY > max) 
            {
                // store largest orientation
                max = sumX*sumX + sumY*sumY;
                orientation = getAngle(sumX, sumY);
            }
        }

        // assign orientation of the dominant response vector
        points[i].orientation = orientation;

        /* This needs a huge amount of time and for the most cases and especially our Demo Application it is not used.
        The Descriptor Values can be written to a file and used for later matching, but this is not mentioned in the paper */
        if(bDes)
        {
            // prepare the orientaton detection of the descriptors
            scale = points[i].scale;
            x = fRound(points[i].x);
            y = fRound(points[i].y);  
            co = cos(points[i].orientation);
            si = sin(points[i].orientation);

            int a = -8;

            // Calculate descriptor for this interest point
            while(a < 12)
            {
                j = -8;
                a = a-4;

                cx += 1.f;
                cy = -0.5f;

                while(j < 12) 
                {
                    dx=dy=mdx=mdy=0.f;
                    cy += 1.f;

                    j = j - 4;

                    ix = a + 5;
                    jx = j + 5;

                    xs = fRound(x + ( -jx*scale*si + ix*scale*co));
                    ys = fRound(y + ( jx*scale*co + ix*scale*si));

                    for (int k = a; k < a + 9; ++k) 
                    {
                        for (int l = j; l < j + 9; ++l) 
                        {
                            //Get coords of sample point on the rotated axis
                            sample_x = fRound(x + (-l*scale*si + k*scale*co));
                            sample_y = fRound(y + ( l*scale*co + k*scale*si));

                            //Get the gaussian weighted x and y responses
                            gauss_s1 = gaussian(xs-sample_x,ys-sample_y,2.5f*scale);
                            rx = haarXImage(sample_y, sample_x, 2*fRound(scale), imageData);
                            ry = haarYImage(sample_y, sample_x, 2*fRound(scale), imageData);

                            //Get the gaussian weighted x and y responses on rotated axis
                            rrx = gauss_s1*(-rx*si + ry*co);
                            rry = gauss_s1*(rx*co + ry*si);

                            dx += rrx;
                            dy += rry;
                            mdx += fabs(rrx);
                            mdy += fabs(rry);

                        }
                    }

                    //Add the values to the descriptor array via the poiter vector
                    gauss_s2 = gaussian(cx-2.0f,cy-2.0f,1.5f);

                    points[i].descriptors[count++] = dx*gauss_s2;
                    points[i].descriptors[count++] = dy*gauss_s2;
                    points[i].descriptors[count++] = mdx*gauss_s2;
                    points[i].descriptors[count++] = mdy*gauss_s2;

                    // set new len
                    len += (dx*dx + dy*dy + mdx*mdx + mdy*mdy) * gauss_s2*gauss_s2;

                    j += 9;
                }
                a += 9;
            }
            count = 0;

            //Convert to Unit Vector
            len = sqrt(len);
            for(int a = 0; a < DescriptorLength; ++a)
            {
                points[i].descriptors[a] /= len;
            }
        }
    }
}
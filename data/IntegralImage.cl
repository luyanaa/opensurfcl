
__constant float cR = 0.2989f;
__constant float cG = 0.5870f;
__constant float cB = 0.1140f;

__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

//aktuelle Version ( mit m_UseImage = true)
__kernel void CreateIntegralImageParallelV3(__read_only image2d_t imageData, __write_only image2d_t result, __global float* buffer, int pixelSize)
{
    size_t id =  get_global_id(0);
	int width = get_image_width(imageData)/pixelSize;
	int height = get_image_height(imageData);
	
	
	for(int y = id; y < height; y+= get_global_size(0)) 
	{	
		float sum = 0;
		for (int x = 0; x < width; x++) //horizontal aufaddieren
		{
			int2 position = {x*pixelSize,y};

			if( pixelSize == 3 || pixelSize == 4)
			{
				uint b = read_imageui(imageData, sampler, position).x;
				position.x = x * pixelSize+1;
				uint g = read_imageui(imageData, sampler, position).x;
				position.x = x * pixelSize+2;
				uint r = read_imageui(imageData, sampler, position).x;
				sum += (cR * r + cG * g + cB * b) / 255.0f;
			}else if( pixelSize == 1)
			{
				float grey = read_imageui(imageData, sampler, position).x / 255.0f;
				sum += grey;
			}

			buffer[y * width + x] = sum;
		}
	}
	
	//lässt ALLE workitems warten.
	//( geht nur innerhalb einer gruppe daher darf nur eine gruppe losgeschickt werden!)
	barrier(CLK_LOCAL_MEM_FENCE);

	for(int x = id; x < width; x+= get_global_size(0))
	{
		float sum = buffer[0 * width + x];
		for (int y = 1; y < height; y++)
		{
			int2 position = {x,y};
			sum += buffer[y * width + x];
			write_imagef(result, position, sum);
		}
	}
}




//aktuelle Version, wenn kein Imagesupport verfügbar ist
__kernel void CreateIntegralImageParallelV2(const unsigned int height,
											const unsigned int width,
											__global const unsigned char* imageData, 
											__global float* result, int pixelSize)
{
    size_t id =  get_global_id(0);
	
	for(int y = id; y < height; y+=get_global_size(0)) 
	{	
		float sum = 0;
		for (int x = 0; x < width; x++) //horizontal aufaddieren
		{
			if(pixelSize == 3 || pixelSize == 4)
			{
				float r = imageData[(y * width + x) * pixelSize + 2];
				float g = imageData[(y * width + x) * pixelSize + 1];
				float b = imageData[(y * width + x) * pixelSize + 0];

				sum += (cR * r + cG * g + cB * b) / 255.0f;
			}else if(pixelSize == 1)
			{
				sum += imageData[(y * width + x)] / 255.0f;
			}
			result[y * width + x] = sum;
		}
	}
	
	//lässt ALLE workitems warten.
	//( geht nur innerhalb einer gruppe daher darf nur eine gruppe losgeschickt werden!)
	barrier(CLK_LOCAL_MEM_FENCE);

	//vertikal aufaddieren
	for(int x = id; x < width; x+= get_global_size(0))
	{
		for (int y = 1; y < height; y++)
		{
			result[y * width + x] += result[(y-1) * width + x];
		}
	}
}
__kernel void CreateIntegralImageParallelV3_24Bit(__read_only image2d_t imageData, __global float* result )
{
    size_t id =  get_global_id(0);
	int width = get_image_width(imageData)/3;
	int height = get_image_height(imageData);
	
	
	for(int y = id; y < height; y+= get_global_size(0)) 
	{	
		float sum = 0;
		for (int x = 0; x < width; x++) //horizontal aufaddieren
		{
			//float2 position = {1/x+0.25f,1/y+0.25f};
			int2 position = {x*3,y};
			//float4 col = read_imagef(imageData, sampler, position);
			uint b = read_imageui(imageData, sampler, position).x;
			position.x = x*3+1;
			uint g = read_imageui(imageData, sampler, position).x;
			position.x = x*3+2;
			uint r = read_imageui(imageData, sampler, position).x;
            
			sum += (cR * r + cG * g + cB * b) / 255.0f;
			//sum = col.x;
			//sum = col.x;
			result[y * width + x] = sum;
		}
	}
	
	//lässt ALLE workitems warten.
	//( geht nur innerhalb einer gruppe daher darf nur eine gruppe losgeschickt werden!)
	barrier(CLK_LOCAL_MEM_FENCE);
	//return;
	//vertikal aufaddieren
	for(int x = id; x < width; x+= get_global_size(0))
	{
		for (int y = 1; y < height; y++)
		{
			result[y * width + x] += result[(y-1) * width + x];
		}
	}
}
__kernel void CreateIntegralImageParallelV3_24BitTwoParts1(__read_only image2d_t imageData, __write_only image2d_t result)
{
    size_t id =  get_global_id(0);
	int width = get_image_width(imageData)/3;
	int height = get_image_height(imageData);
	
	
	for(int y = id; y < height; y+= get_global_size(0)) 
	{	
		float sum = 0;
		for (int x = 0; x < width; x++) //horizontal aufaddieren
		{
			//float2 position = {1/x+0.25f,1/y+0.25f};
			int2 position = {x*3,y};
			//float4 col = read_imagef(imageData, sampler, position);
			uint b = read_imageui(imageData, sampler, position).x;
			position.x = x*3+1;
			uint g = read_imageui(imageData, sampler, position).x;
			position.x = x*3+2;
			uint r = read_imageui(imageData, sampler, position).x;
            
			sum += (cR * r + cG * g + cB * b) / 255.0f;

			position.x = x;
			write_imagef(result, position, sum);
		}
	}
}

__kernel void CreateIntegralImageParallelV3_24BitTwoParts2(__read_only image2d_t imageData, __write_only image2d_t result)
{
    size_t id =  get_global_id(0);
	int width = get_image_width(imageData);
	int height = get_image_height(imageData);

	for(int x = id; x < width; x+= get_global_size(0))
	{
		int2 position = {x,0};
		float sum = read_imagef(imageData, sampler, position).x;
		write_imagef(result, position, sum);
		for (int y = 1; y < height; y++)
		{
			position.y = y;
			sum += read_imagef(imageData, sampler, position).x;
			write_imagef(result, position, sum);
		}
	}
}

//Testversion zum Speichern in ein Image Speed: 0,014 microsec pro pixel( langsamer als in float buffer speichern!)
__kernel void CreateIntegralImageParallelV3_24Bit_(__read_only image2d_t imageData, __write_only image2d_t result, __global float* buffer )
{
    size_t id =  get_global_id(0);
	int width = get_image_width(imageData)/3;
	int height = get_image_height(imageData);
	
	
	for(int y = id; y < height; y+= get_global_size(0)) 
	{	
		float sum = 0;
		for (int x = 0; x < width; x++) //horizontal aufaddieren
		{
			//float2 position = {1/x+0.25f,1/y+0.25f};
			int2 position = {x*3,y};
			//float4 col = read_imagef(imageData, sampler, position);
			uint b = read_imageui(imageData, sampler, position).x;
			position.x = x*3+1;
			uint g = read_imageui(imageData, sampler, position).x;
			position.x = x*3+2;
			uint r = read_imageui(imageData, sampler, position).x;
            
			sum += (cR * r + cG * g + cB * b) / 255.0f;
			//sum = col.x;
			//sum = col.x;
			buffer[y * width + x] = sum;
			//position.x = x;
			//write_imagef(result, position, sum);
		}
	}
	
	//lässt ALLE workitems warten.
	//( geht nur innerhalb einer gruppe daher darf nur eine gruppe losgeschickt werden!)
	barrier(CLK_LOCAL_MEM_FENCE);

	for(int x = id; x < width; x+= get_global_size(0))
	{
		float sum = buffer[0 * width + x];
		for (int y = 1; y < height; y++)
		{
			int2 position = {x,y};
			sum += buffer[y * width + x];
			write_imagef(result, position, sum);
			//result[y * width + x] += result[(y-1) * width + x];
		}
	}
}



//veraltet
__kernel void CreateIntegralImageParallel(const unsigned int height,
											const unsigned int width,
											__global const unsigned char* imageData, 
											__global float* result )
{
    size_t id =  get_global_id(0);
	//Bei hohen Bildern muss ein waagerecht arbeitendes Workitem mehrere Zeilen bearbeiten
	size_t linesPerItem = ceil(height / (float)get_local_size(0));
    //Bei breiten Bildern muss ein senkrecht arbeitendes Workitem mehrere Spalten bearbeiten
	size_t colsPerItem = ceil(width / (float)get_local_size(0));
	
	
	
	for(int y = id*linesPerItem; y < id*linesPerItem + linesPerItem  && y < height; y++)
	{	
		float sum = 0;
		for (int x = 0; x < width; x++) //horizontal aufaddieren
		{
			float r = imageData[(y * width + x) * 3 + 2];
			float g = imageData[(y * width + x) * 3 + 1];
			float b = imageData[(y * width + x) * 3 + 0];
            
			sum += (cR * r + cG * g + cB * b) / 255.0f;

			result[y * width + x] = sum;
		}
	}
	
	//lässt ALLE workitems warten.
	//( geht nur innerhalb einer gruppe daher darf nur eine gruppe losgeschickt werden!)
	barrier(CLK_LOCAL_MEM_FENCE);

	//vertikal aufaddieren
	for(int x = id*colsPerItem; x < id * colsPerItem +  colsPerItem && x < width; x++)
	{
		for (int y = 1; y < height; y++)
		{
			result[y * width + x]+= result[(y-1) * width + x];
		}
		
	}
}

__kernel void CreateIntegralImageParallelV3_32Bit(__read_only image2d_t imageData, __global float* result )
{
    size_t id =  get_global_id(0);
	int width = get_image_width(imageData);
	int height = get_image_height(imageData);
	
	for(int y = id; y < height; y+= get_global_size(0)) 
	{	
		float sum = 0;
		for (int x = 0; x < width; x++) //horizontal aufaddieren
		{
			int2 position = {x,y};
			float4 col = read_imagef(imageData, sampler, position);
            
			sum += (cR * col.x + cG * col.y + cB * col.z);
			//sum = col.x;
			result[y * width + x] = sum;
		}
	}
	
	//lässt ALLE workitems warten.
	//( geht nur innerhalb einer gruppe daher darf nur eine gruppe losgeschickt werden!)
	barrier(CLK_LOCAL_MEM_FENCE);

	//vertikal aufaddieren
	for(int x = id; x < width; x+= get_global_size(0))
	{
		for (int y = 1; y < height; y++)
		{
			result[y * width + x] += result[(y-1) * width + x];
		}
	}
}


__kernel void CreateIntegralImageParallelTwoParts1H(const unsigned int height,
											const unsigned int width,
											__global unsigned char* imageData, 
											__global float* result )
{
    size_t id =  get_local_id(0);
	
	
	//for(int y = id*linesPerItem; y < id*linesPerItem + linesPerItem  && y < height; y++)
	for(int y = id; y < height; y+=get_global_size(0)) // 0,008 microsec schneller pro pixel!!!
	{	
		float sum = 0;
		for (int x = 0; x < width; x++) //horizontal aufaddieren
		{
			float r = imageData[(y * width + x) * 3 + 2];
			float g = imageData[(y * width + x) * 3 + 1];
			float b = imageData[(y * width + x) * 3 + 0];
			sum += (cR * r + cG * g + cB * b) / 255.0f;


			result[y * width + x] = sum;
		}
	}
}

__kernel void CreateIntegralImageParallelTwoParts2H(const unsigned int height,
											const unsigned int width,
											__global unsigned char* imageData, 
											__global float* result )
{
    size_t id =  get_local_id(0);

	//vertikal aufaddieren
	//for(int x = id*colsPerItem; x < id * colsPerItem +  colsPerItem && x < width; x++)
	for(int x = id; x < width; x+= get_global_size(0))
	{
		for (int y = 1; y < height; y++)
		{
			result[y * width + x]+= result[(y-1) * width + x];
		}
	}
}




__kernel void CreateIntegralImageParallelTwoParts1V(const unsigned int height,
											const unsigned int width,
											__global unsigned char* imageData, 
											__global float* result )
{
    size_t id =  get_local_id(0);
	
	
	for(int x = id; x < width; x+= get_global_size(0))
	{	
		float sum = 0;
		for (int y = 0; y < height; y++) //vertikal aufaddieren
		{
			float r = imageData[(y * width + x) * 3 + 2];
			float g = imageData[(y * width + x) * 3 + 1];
			float b = imageData[(y * width + x) * 3 + 0];
			sum += (cR * r + cG * g + cB * b) / 255.0f;

			result[y * width + x] = sum;
		}
	}
}

__kernel void CreateIntegralImageParallelTwoParts2V(const unsigned int height,
											const unsigned int width,
											__global unsigned char* imageData, 
											__global float* result )
{
    size_t id =  get_local_id(0);
	
	//vertikal aufaddieren
	//for(int x = id*colsPerItem; x < id * colsPerItem +  colsPerItem && x < width; x++)
	for(int y = id; y < height; y+=get_global_size(0)) // 0,008 microsec schneller pro pixel!!!
	{
		for (int x = 1; x < width; x++)
		{
			result[y * width + x]+= result[y * width + (x-1)];
		}
	}
}



//CPU Sequencialversion on GPU
__kernel void CreateIntegralImageSequencial(const unsigned int height, 
											const unsigned int width, 
											__global unsigned char* imageData, 
											__global float* result )
{
	float rowsum = 0;
    for (int x = 0; x < width; x++)
    {
        float r = imageData[x*3 + 2];
        float g = imageData[x*3 + 1];
        float b = imageData[x*3];
        rowsum += (cR * r + cG * g + cB * b) / 255.0f;
        result[x] = rowsum;
    }
    for (int y = 1; y < height; y++)
    {
        rowsum = 0;
        for (int x = 0; x < width; x++)
        {
            float r = imageData[(y * width + x) * 3 + 2];
            float g = imageData[(y * width + x) * 3 + 1];
            float b = imageData[(y * width + x) * 3];
            
            rowsum += (cR * r + cG * g + cB * b) / 255.0f;
            
            // integral image is rowsum + value above
            result[y * width + x] = rowsum + result[(y - 1) * width + x];
        }
    }
}

//durchläuft pro aufruf eine Zeile / es gibt Kollisionen weil parallel auf den gleichen Speicher
//zugegriffen wird
__kernel void CreateIntegralImageParallelV1(const unsigned int height, 
											const unsigned int width, 
											const unsigned int offset,
											__global unsigned char* imageData, 
											__global float* result )
{
    size_t globalID =  get_global_id(0)+offset;
    if(globalID >= height)
    {
		return;
    }
	
	float rowsum = 0;
	int y = globalID;
	rowsum = 0;
    for (int x = 0; x < width; x++)
    {
        float r = imageData[(y * width + x) * 3 + 2];
        float g = imageData[(y * width + x) * 3 + 1];
        float b = imageData[(y * width + x) * 3 + 0];
            
        rowsum += (cR * r + cG * g + cB * b) / 255.0f;
		
		if( y == 0)
		{
			result[y * width + x] = rowsum;
		}
		else
		{
			result[y * width + x] = rowsum + result[(y - 1) * width + x];
		}
            
    }
}
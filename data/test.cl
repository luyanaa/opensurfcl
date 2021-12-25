
__kernel void TestCL(__global float* buffer)
{
	buffer[0] = buffer[0] * 2;
}

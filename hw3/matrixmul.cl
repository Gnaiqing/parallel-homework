__kernel void matrix_vector_mul(__global const float* src_a,__global const float * src_b,__global float* dst,const size_t m,const size_t n)
{
	const int idx= get_global_id(0);
	if(idx<m)
	{
		dst[idx]=0;
        for(int i=0;i<n;i++)dst[idx]+=src_a[idx*n+i]*src_b[i];
	}
}
// TODO: Add OpenCL kernel code here.
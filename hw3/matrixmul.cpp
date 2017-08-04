#include<stdio.h>
#include<cl.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#define MEM_SIZE (128)
#define MAX_SOURCE_SIZE (0x100000)
/*in the temp test case,m*n<=10^7 and n<=10^4 is required*/ 
#define M_SIZE (3000)
#define N_SIZE (3000)
size_t shrRoundUp(size_t f , size_t s)
{
    return (s+f-1)/f*f;
}
int main(){
	time_t start_time=time(NULL);
	cl_device_id device_id=NULL;
	cl_context context=NULL;
	cl_command_queue command_queue=NULL;
	cl_mem mem_a=NULL,mem_b=NULL,mem_d=NULL;
	cl_program program=NULL;
	cl_kernel kernel=NULL;
	cl_platform_id platform_id=NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret;
	float* src_a,*src_b,*dst;
	src_a=(float*)malloc(M_SIZE*N_SIZE*sizeof(float));
	src_b=(float*)malloc(N_SIZE*sizeof(float));
	dst=(float*)malloc(M_SIZE*sizeof(float));

	FILE * fp;
	char fileName[]="./matrixmul.cl";
	char* source_str;
	size_t source_size;
	/*Load the source code containing the kernel*/

	fp=fopen(fileName,"r");
	if(!fp){
		fprintf(stderr,"Failed to load kernel.\n");
		exit(1);
	}
	source_str =(char*)malloc(MAX_SOURCE_SIZE);
	source_size=fread(source_str,1,MAX_SOURCE_SIZE,fp);
	fclose(fp);

	/* Load input Matrix and Vector*/
	size_t m=M_SIZE;
	size_t n=N_SIZE;
    fp=fopen("./matrix.txt","r");
	for(size_t i=0;i<m;i++)
		for(size_t j=0;j<n;j++)
			fscanf(fp,"%f",&src_a[i*n+j]);
	fclose(fp);
	fp=fopen("./vector.txt","r");
	for(size_t i=0;i<n;i++)
		fscanf(fp,"%f",&src_b[i]);
	fclose(fp);

	/*Get Platform and Device info*/
	ret=clGetPlatformIDs(1,&platform_id,&ret_num_platforms);
	ret=clGetDeviceIDs(platform_id,CL_DEVICE_TYPE_GPU,1,&device_id,
		&ret_num_devices);
	size_t ans;
	ret=clGetDeviceInfo(device_id,CL_DEVICE_MAX_COMPUTE_UNITS,sizeof(size_t),&ans,NULL);
	printf("max compute units =%d\n",ans);
	ret=clGetDeviceInfo(device_id,CL_DEVICE_MAX_WORK_GROUP_SIZE,sizeof(size_t),&ans,NULL);
	printf("max work_item in a group =%d\n",ans);
	
	/*Create OpenCL context*/
	context=clCreateContext(NULL,1,&device_id,NULL,NULL,&ret);

	/*Create Command Queue*/
	command_queue= clCreateCommandQueue(context,device_id,CL_QUEUE_PROFILING_ENABLE,&ret);

	/*Create Memory Buffer*/
	mem_a=clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,
		M_SIZE*N_SIZE*sizeof(float),src_a,&ret);
	mem_b=clCreateBuffer(context,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,
		N_SIZE*sizeof(float),src_b,&ret);
	mem_d=clCreateBuffer(context,CL_MEM_WRITE_ONLY,
		M_SIZE*sizeof(float),NULL,&ret);

	/*create Kernel Program from the source*/
	program= clCreateProgramWithSource(context,1,(const char**)&source_str,
		(const size_t*) &source_size,&ret);
	/*Build Kernel Program*/
	ret=clBuildProgram(program,1,&device_id,NULL,NULL,NULL);
	/*Create OpenCL Kernel*/
	kernel = clCreateKernel(program,"matrix_vector_mul",&ret);
	ret=clGetKernelInfo(kernel,CL_KERNEL_WORK_GROUP_SIZE,sizeof(size_t),&ans,NULL);
	printf("kernel:max work_item in a group =%d\n",ans);
	/*set OpenCL Kernel Parameters*/
	
	ret=clSetKernelArg(kernel,0,sizeof(cl_mem),(void*)&mem_a);
	ret=clSetKernelArg(kernel,1,sizeof(cl_mem),(void*)&mem_b);
	ret=clSetKernelArg(kernel,2,sizeof(cl_mem),(void*)&mem_d);
	ret=clSetKernelArg(kernel,3,sizeof(int),(void*)&m);
	ret=clSetKernelArg(kernel,4,sizeof(int),(void*)&n);
	const size_t local_ws=128;
	const size_t global_ws=shrRoundUp(local_ws,m);
	clFinish(command_queue);
	cl_event event;
	ret=clEnqueueNDRangeKernel(command_queue,kernel,1,NULL,&global_ws,&local_ws
		,0,NULL,&event);
	if(ret!=CL_SUCCESS){
		printf("execute error\n");
		exit(ret);
	}
	clWaitForEvents(1,&event);
	cl_ulong time_start, time_end;
    double calc_time;
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
	calc_time=time_end-time_start;
	ret=clEnqueueReadBuffer(command_queue,mem_d,CL_TRUE,0,sizeof(float)*m,
		dst,0,NULL,NULL);
	printf("problem size = %d:%d\n",m,n);
	printf("first 10 elements are: ");
	for(int i=0;i<10;i++)printf("%0.3f ",dst[i]);
	printf("\nlast 10 elements are: ");
	for(int i=m-10;i<m;i++)printf("%0.3f ",dst[i]);
	printf("\n");
	ret=clFlush(command_queue);
	ret=clFinish(command_queue);
	ret=clReleaseKernel(kernel);
	ret=clReleaseProgram(program);
	ret=clReleaseMemObject(mem_a);
	ret=clReleaseMemObject(mem_b);
	ret=clReleaseMemObject(mem_d);
	ret=clReleaseCommandQueue(command_queue);
	ret=clReleaseContext(context);

	free(source_str);
	free(src_a);
	free(src_b);
	free(dst);
	time_t finish_time=time(NULL);
	
	double stime=difftime(finish_time,start_time);
	printf("tot time is %0.3f s\ncalc time is %0.3f ms\n",stime,calc_time/1000000);
	return 0;
}

#include<stdio.h>
#include<mpi.h>
const int MAX_SIZE=1000000;
const int TEST_TIME=10;
int main(int argc,char* argv[]){
    int a[MAX_SIZE];
    int id;
    int p;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&id);
    MPI_Comm_size(MPI_COMM_WORLD,&p);
    if(argc!=2){
        printf("Command line:%s <m>\n",argv[0]);
        MPI_Finalize();
        return 1;
    }
    if(id==0){
        for(int i=0;i<MAX_SIZE;i++)a[i]=i+1;
    }
    double tot_time=0;
    int len=atoi(argv[1]);
    for(int i=0;i<TEST_TIME;i++){
        MPI_Status mpis;
        MPI_Barrier(MPI_COMM_WORLD);
        double elapsed_time=-MPI_Wtime();
        if(id==0){
            MPI_Send(a,len,MPI_INT,1,0,MPI_COMM_WORLD);
        }
        if(id==1){
            MPI_Recv(a,len,MPI_INT,0,0,MPI_COMM_WORLD,&mpis);
        }
        elapsed_time+=MPI_Wtime();
        tot_time+=elapsed_time;
    }
    tot_time/=TEST_TIME;
    double bandwidth=len/(1000000*tot_time);
    if(id==0){
        printf("message length=%d, average time=%10.6f s\n",len,tot_time);
	printf("bandwidth=%11.6f megabits per sec\n",bandwidth);
    }
    MPI_Finalize();
    return 0;
}

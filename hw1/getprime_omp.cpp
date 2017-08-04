#define _CRT_SECURE_NO_WARNINGS
/* 
* This program uses the Sieve of Eratosthenes to determine the 
* number of prime numbers less than or equal to 'n'. 
* 
* Adapted from code appearing in "Parallel Programming in C with 
* MPI and OpenMP," by Michael J. Quinn, McGraw-Hill (2004). 
*/ 
#include <stdio.h> 
#include <stdlib.h> 
#include <omp.h>
#include <time.h>
#include <math.h>
int main (int argc, char *argv[]) 
{ 
	int thread_num;
	printf("max thread_num is %d\n",omp_get_max_threads());
	printf("please enter thread_num\n");
	scanf("%d",&thread_num);
	omp_set_num_threads(thread_num);
	clock_t start,finish;
	int count; /* Prime count */ 
	int first; /* Index of first multiple */ 
	int i; 
	int index; /* Index of current prime */ 
	char *marked; /* Marks for 3,5,...,'n|1 ' */ 
	long long int n; /* Sieving from 2, ..., 'n' */ 
	long long int N; /* Size of sieve and loop bounds */
	int sqrtN;/*Size of smaller sieve and loop bounds*/
	int prime; /* Current prime */ 
	int *primeList;
	int listSize;/*num of prime less than sqrt(N)*/
	if (argc != 2) { 
		printf ("Command line: %s <m>\n", argv[0]); 
		exit (1); 
	} 
	n = atoi(argv[1]); 
	/*{
		printf("enter n\n");
		scanf("%d",&n);
	}*/
	N = n+1; 
	sqrtN=sqrt(1.0*n)+1;
	marked = (char *) malloc (N); //alocate slots for numbers in range [0,n] 
	if (marked == NULL) { 
		printf ("Cannot allocate enough memory\n"); 
		exit (1); 
	} 
	primeList = (int*)malloc(sizeof(int)*sqrtN);
#pragma omp parallel
	{
		printf("hello world\n");
	}
	start=clock();
#pragma omp parallel for
	for (i = 1; i < (N >> 1); i++) marked[i] = 1; //marked i
	
	marked[0]=0;
	index = 1; 
	prime = 3; 
    listSize=0;
	do {
		primeList[listSize++]=prime;
		first=prime*prime;
		for (i = first; i < sqrtN; i += prime*2) marked[i>>1] = 0; 	
		while (!marked[++index]) ; 
		prime = index*2+1; 
	} while (prime * prime < sqrtN); 
	for(; index*2+1 < sqrtN; index++)if(marked[index]) primeList[listSize++] = index*2+1;
	;
#pragma omp parallel for private(prime,first) schedule(dynamic,1)
	for(int k = 0;k < listSize;k++){
		prime=primeList[k];
		first=prime*prime;
		for(int i = first; i < N; i += prime*2) marked[i>>1] = 0;/*to make sure i is odd*/
	}
	finish=clock();
	printf("calc time=%f\n",(double)(finish-start)/(CLOCKS_PER_SEC)); 
	count = 1; 
	for (i = 0; i < (N >> 1); i++) 
		count += marked[i]; 
	printf ("There are %d primes less than or equal to %d\n\n", count, n); 
	return 0; 
}


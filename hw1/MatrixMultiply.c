/*
 *   Matrix multiplication
 */

#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
/*
 *   Function 'rerror' is called when the program detects an
 *   error and wishes to print an appropriate message and exit.
 */

void rerror (char *s)
{
   printf ("%s\n", s);
   exit (-1);
}

/*
 *   Function 'allocate_matrix", passed the number of rows and columns,
 *   allocates a two-dimensional matrix of floats.
 */   

void allocate_matrix (float ***subs, int rows, int cols) {
   int    i;
   float *lptr, *rptr;
   float *storage;

   storage = (float *) malloc (rows * cols * sizeof(float));
   *subs = (float **) malloc (rows * sizeof(float *));
   for (i = 0; i < rows; i++)
      (*subs)[i] = &storage[i*cols];
   return;
}

/*
 *   Given the name of a file containing a matrix of floats, function
 *   'read_matrix' opens the file and reads its contents.
 */

void read_matrix (
   char    *s,          /* File name */
   float ***subs,       /* 2D submatrix indices */
   int     *m,          /* Number of rows in matrix */
   int     *n)          /* Number of columns in matrix */
{
   char     error_msg[80];
   FILE    *fptr;          /* Input file pointer */

   fptr = fopen (s, "r");
   if (fptr == NULL) {
      sprintf (error_msg, "Can't open file '%s'", s);
      rerror (error_msg);
   }
   fread (m, sizeof(int), 1, fptr);
   fread (n, sizeof(int), 1, fptr);
   allocate_matrix (subs, *m, *n);
   fread ((*subs)[0], sizeof(float), *m * *n, fptr);
   fclose (fptr);
   return;
}

/*
 *   Passed a pointer to a two-dimensional matrix of floats and
 *   the dimensions of the matrix, function 'print_matrix' prints
 *   the matrix elements to standard output. If the matrix has more
 *   than 10 columns, the output may not be easy to read.
 */

void print_matrix (float **a, int rows, int cols)
{
   int i, j;

   for (i = 0; i < rows; i++) {
      for (j = 0; j < cols; j++)
	 printf ("%6.2f ", a[i][j]);
      putchar ('\n');
   }
   putchar ('\n');
   return;
}

/*
 *   Function 'matrix_multiply' multiplies two matrices containing
 *   floating-point numbers.
 */

void matrix_multiply (float **a, float **b, float **c,
		      int arows, int acols, int bcols)
{
   clock_t start,finish;
   double mul_time;
   start=clock();
   #pragma omp parallel for schedule(dynamic,1)
   for (int i = 0; i < arows; i++){
      for (int j = 0; j < bcols; j++) {
	  float tmp = 0.0;
	  for (int k = 0; k < acols; k++)
	    tmp += a[i][k] * b[k][j];
	  c[i][j] = tmp;
      }
   }
   finish=clock();
   mul_time=(double)(finish-start)/(CLOCKS_PER_SEC);
   printf("multiply_time=%f secs\n",mul_time);
   return;
}

int main (int *argc, char *argv[])
{
   int m1, n1;        /* Dimensions of matrix 'a' */
   int m2, n2;        /* Dimensions of matrix 'b' */
   float **a, **b;    /* Two matrices being multiplied */
   float **c;         /* Product matrix */
   
   int thread_num;
   printf("max thread=%d\n",omp_get_max_threads());
   printf("please enter thread_num\n");
   scanf("%d",&thread_num);
   omp_set_num_threads(thread_num);
  
   read_matrix ("matrix_a", &a, &m1, &n1);
   //print_matrix (a, m1, n1);
   read_matrix ("matrix_b", &b, &m2, &n2);
   //print_matrix (b, m2, n2);
   if (n1 != m2) rerror ("Incompatible matrix dimensions");
   allocate_matrix (&c, m1, n2);
   matrix_multiply (a, b, c, m1, n1, n2);
   //print_matrix (c, m1, n2);
   return 0;
}

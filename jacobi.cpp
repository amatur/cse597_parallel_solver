#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "matrix_util.h"

void jacobiSolve(int n, double** A, double* b, double* x, double eps = 1e-10, int maxit = 100)
{
	//int maxit = 2*n*n;
	int numit;
	
    double *dx,*y;
    dx = (double*) calloc(n,sizeof(double));
    y = (double*) calloc(n,sizeof(double));
    int i,j,k;

	//random initialization
	for(int j=0; j<n; j++)
	{
		x[j] = (double)rand()/(double)(RAND_MAX)*1.0;
    }
    
    
    // Note that we go through to our max iterations
    for(k=0; k<maxit; k++)
    {
        double totSum = 0.0;
        double localSum = 0.0;
        double localInd = 0.0;
        for(i=0; i<n; i++)
        {
            dx[i] = b[i];
            for(j=0; j<n; j++)
            {
                dx[i] -= A[i][j]*x[j]; 
            }
            dx[i] /= A[i][i];
            y[i] += dx[i];
            
            // Create a residual from part of the domain (when the indices  are 0, 5, 10, ...)
            if ( (i % 5) == 0)
            {
                localSum += ( (dx[i] >= 0.0) ? dx[i] : -dx[i]);

            }

            // Create a residual from a single point
            if (i == n/2)
            {
                localInd += ( (dx[i] >= 0.0) ? dx[i] : -dx[i]);
            }

            // Create a residual over all of the domain
            totSum += ( (dx[i] >= 0.0) ? dx[i] : -dx[i]);
        }
       
        // Update x
        for(i=0; i<n; i++) x[i] = y[i];
        
        // Print the residuals to the screen
        printf("%4d\t%.3e\t%.3e\t%.3e \n",k,totSum,localSum,localInd);

        // Break out if we reach our desired tolerance
        if(totSum <= eps) break;
    }
    numit = k+1;
    free(dx); free(y);
}


//~ 
//~ bool testJacobi(){
    //~ // simple 4 by 4 input matrix
    //~ double A[4*4] = {10., -1., 2., 0.,
                           //~ -1., 11., -1., 3.,
                           //~ 2., -1., 10., -1.,
                           //~ 0.0, 3., -1., 8.};
    //~ double b[4] =  {6., 25., -11., 15.};
    //~ double x[4];
    //~ double expected_x[4] = {1.0,  2.0, -1.0, 1.0};
    //~ int n = 4;
//~ 
    //~ // testing sequential matrix multiplication
    //~ jacobi(n, A, b, x);
//~ 
    //~ // checking if all values are correct (up to some error value)
    //~ for (int i = 0; i < n; ++i)
    //~ {
        //~ EXPECT_NEAR(expected_x[i], x[i], 1e-10) << " element y[" << i << "] is wrong";
    //~ }
//~ }
	//~ 
//~ }


/**
 * @brief   Randomly Initialize the A matrix
 */
void fillA_random(double **A, int n){
	int countA,countB;
	for (countA=0; countA<n; countA++) 
	{
		for (countB=0; countB<n; countB++) 
		{
			A[countA][countB]=(double)rand()/(double)(RAND_MAX)*1.0;
		}
	}
}

void fillA_poisson(double **A, int n){
    
    for(int i = 0; i<n*n; i++){
         for(int j = 0; j<n*n; j++){
            if(i==j){
                A[i][j] = 4;
            }else if (i == j+1  || i == j -1 || i==j+n || i==j-n){
                A[i][j] = -1;
            }
        }
    }
    
    for(int i = n-1; i<n*n-1; i = i + n){
        for(int j = n-1; j<n*n-1; j = j + n){
            A[i+1][j] = 0;
            A[i][j+1] = 0;
        }
    }
    
}


void sparse(double **sparseMatrix, int dim, double **compactMatrix){

//~ Two vectors, i and j, that specify the row and column subscripts
//~ One vector, s, containing the real or complex data you want to store in the sparse matrix. Vectors i, j and s should all have the same length.
//~ Two scalar arrays, m and n, that specify the dimensions of the sparse matrix to be created
//~ An optional scalar array that specifies the maximum amount of storage that can be allocated for this sparse array

    int size = 0; 
    for (int i = 0; i < dim; i++) 
        for (int j = 0; j < dim; j++) 
            if (sparseMatrix[i][j] != 0) 
                size++;
    compactMatrix = (double**) calloc(3 ,sizeof(double*));
    for(int i = 0; i < 3; i++){
		compactMatrix[i] = (double*) calloc(size,sizeof(double));
		
	}
    
    
    // Making of new matrix 
    int k = 0; 
    for (int i = 0; i < dim; i++) 
        for (int j = 0; j < dim; j++) 
            if (sparseMatrix[i][j] != 0) 
            { 
                compactMatrix[0][k] = i; 
                compactMatrix[1][k] = j; 
                compactMatrix[2][k] = sparseMatrix[i][j]; 
                k++; 
            } 
  
    for (int i=0; i<3; i++) 
    { 
        for (int j=0; j<size; j++) 
            printf("%lf ", compactMatrix[i][j]); 
  
        printf("\n"); 
    } 
}         
               
                

/*
int main(){
    int n = 2;
    
    int N = n*n;
    double **A;
    double *b, *x;
    
	srand(0);
	
	
    clock_t start, end;
    start = clock();		//time count starts 
    
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
	// Allocate memory for A matrix 
	 //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
	//~ A = (double**) calloc(N,sizeof(double*));
    //~ A2 = (double**) calloc(N,sizeof(double*));
    //~ for(int i =0; i <N; i++){
		//~ A[i] = (double*) calloc(N,sizeof(double));
		//~ A2[i] = (double*) calloc(N,sizeof(double));
		//~ 
	//~ }
	init2d(&A, N);
    fillA_poisson(A, n);
    
    init1d(&b, N);
    init1d(&x, N);
    
    printf("A Matrix: \n");
    print(A, N);
    
    printf("b Matrix: ");
    print(b, N);
    
    printf("x Matrix: ");
    print(x, N);
    
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
	// Set our tolerance and maximum iterations
    double eps = 1.0e-4;
    int maxit = 2*N*N;
    
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
	// Run jacobi
	jacobiSolve(N, A, b, x, eps, maxit);
	printf("x Matrix: ");
    print(x, N);
    
    end = clock();//time count stops 
    
    double total_time = ((double) (end - start))/ CLOCKS_PER_SEC ;//calulate total time
    
    printf("nTime taken: %.20lf seconds.\n", total_time); //in seconds
    
    
    
    return 0;
}
*/


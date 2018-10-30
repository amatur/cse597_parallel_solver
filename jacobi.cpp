#include <mpi.h>
#include <stdio.h>		
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <cstring>
#include "matrix_util.h"
using namespace std;

int block_decompose(const int n, const int p, const int rank)
{
    return n / p + ((rank < n % p) ? 1 : 0);
}

int block_decompose(const int n, MPI_Comm comm)
{
    int rank, p;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &p);
    return block_decompose(n, p, rank);
}

int block_decompose_by_dim(const int n, MPI_Comm comm, int dim)
{
    // get dimensions
    int dims[2];
    int periods[2];
    int coords[2];
    MPI_Cart_get(comm, 2, dims, periods, coords);
    return block_decompose(n, dims[dim], coords[dim]);
}

void matmul(double **A, double *B, double *C, int Dim){
	int i, k;
	for (i = 0; i < Dim; i++){
		C[i] = 0;
		for (k = 0; k < Dim; k++){
			C[i] += A[i][k] * B[k];
		}
	}	
		
}



double getError(double **A, double* B, double *C, double *x, int Dim){
	matmul(A, x, C, Dim);
	double sum = 0.0;
	for (int i = 0; i<Dim; i++) {
		sum += (C[i] - x[i])*(C[i] - x[i]);
	}
	sum = sqrt(sum);
	return sum;
}


void jacobiSolve(int n, double** A, double* B, double* x, double eps = 1e-10, int maxit = 100){
	memset(x, 0, n*sizeof(*x)); //init guess
	
	//random initialization
	for(int j=0; j<n; j++)
	{
		x[j] = (double)rand()/(double)(RAND_MAX)*1.0;
    }
    
	double* sigma = (double*) calloc(n,sizeof(double));
	
	double* y = (double*) calloc(n,sizeof(double));
	
	double *C = (double *) malloc( n * sizeof(double));
	int it = 0;
	
	int k = 0;
	do{
		it++;
		
		double totSum = 0.0;
        double localSum = 0.0;
        double localInd = 0.0;
		for (int i=0; i<n; i++) {
			sigma[i] = B[i];
			for (int j = 0; j < n; j++) {
				//if(j!=1){
					sigma[i] -= A[i][j] * x[j];
					
				//}
			}
			sigma[i] /= A[i][i];
			
			y[i] += sigma[i];
			
			
			
			// Create a residual from part of the domain (when the indices  are 0, 5, 10, ...)
            if ( (i % 5) == 0)
            {
                localSum += ( (sigma[i] >= 0.0) ? sigma[i] : -sigma[i]);

            }

            // Create a residual from a single point
            if (i == n/2)
            {
                localInd += ( (sigma[i] >= 0.0) ? sigma[i] : -sigma[i]);
            }

            // Create a residual over all of the domain
            totSum += ( (sigma[i] >= 0.0) ? sigma[i] : -sigma[i]);
		}
		k = k + 1;
		//print(x, n);
		//printf("%f", getError(A, B, C, x, n));
		
		
       
        // Update x
        for(int i=0; i<n; i++) x[i] = y[i];
        
        // Print the residuals to the screen
        //printf("%4d\t%.3e\t%.3e\t%.3e \n",k,totSum,localSum,localInd);
        //getError(A, B, C, x, n)
		if(totSum <=eps || it >= maxit){
			break;
		}
		
		
	}while(true);
	
	
}



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
               
                


int main(int argc, char* argv[]){
    int world_rank, world_size;    

   //init mpi 
    MPI_Init(&argc, &argv);
    
    int n = strtol(argv[1], NULL, 10);
   // get communicator size
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &world_size);
    MPI_Comm_rank(comm, &world_rank);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    printf("I am (rank %d) of %d size\n\n", world_rank, world_size);


    // create 2D cartesian grid for the processors (enable reordering)
      int q = (int)sqrt(world_size);
      MPI_Comm grid_comm;
      int dims[2] = {q, q};
      int periods[2] = {0, 0};
      MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &grid_comm);
      // get the rank of process with coordinates (0,0)
      int rank00;
      int myrank;
      int coords[2] = {0, 0};
      MPI_Cart_rank(grid_comm, coords, &rank00);
      MPI_Comm_rank(grid_comm, &myrank);

    //if I am process (0,0) I load A, B and gen x
    if (myrank == rank00){
             
        int N = n*n;
        double **A;
        double *b, *x;
        
        srand(0);
        
        
        clock_t start, end;
        //start = clock();		//time count starts 
        
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
        
        
        //printf("A Matrix: \n");
        //print(A, N);
        
        //printf("b Matrix: ");
        //print(b, N);
        
        //printf("x Matrix: ");
        //print(x, N);
        
        //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
        // Set our tolerance and maximum iterations
        double eps = 1.0e-4;
        int maxit = 2*N*N;
   
        init2d(&A, N);
        fillA_poisson(A, n);
        
        init1d(&b, N);
        init1d(&x, N);
        
        //fill b
        for(int i =0; i<N; i++)
        {
            b[i] = (double)rand()/(double)((RAND_MAX)*1.0);
        }
	
        MPI_Bcast(&N, 1, MPI_INT, rank00, grid_comm);

        jacobiSolve(N, A, b, x, eps, maxit);
    }


    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
	// Run jacobi
	double t_start, t_end, time_secs;
 
    // start timer
    t_start = MPI_Wtime();
	t_end = MPI_Wtime();
    time_secs = t_end - t_start;
    
    cout<< time_secs << " seconds." << endl;
    MPI_File fh; 
    MPI_File_open(MPI_COMM_WORLD, "time.out",
        MPI_MODE_CREATE|MPI_MODE_WRONLY,
        MPI_INFO_NULL, &fh); 
    
    MPI_File_write(fh, &time_secs, 1, MPI_DOUBLE, MPI_STATUS_IGNORE); 
    MPI_File_close(&fh);    

    //printf("x Matrix: ");
    //print(x, N);
    
    //end = clock();//time count stops 
    
    //double total_time = ((double) (end - start))/ CLOCKS_PER_SEC ;//calulate total time
    
    //printf("nTime taken: %.20lf seconds.\n", total_time); //in seconds
    
    MPI_Finalize();
    
    return 0;
}





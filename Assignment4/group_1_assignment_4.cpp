



#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#define N 100               // size of matrix
#define P 4                 // number of processes
#define SQRT_P 2            // square root of P
#define SUB_SIZE N / SQRT_P // size of sub matrix

int main(int argc, char **argv)
{

    /*initialize required variables*/
    int rank, size;

    // sub matrix for D
    double D[SUB_SIZE][N] = {0};

    // sub matrix for D0
    double D0[SUB_SIZE][N];

    // buffer for sending submatrices
    double sendbuf[SUB_SIZE * N] = {0};

    // buffer for receiving submatrices
    double recvbuf[SUB_SIZE * N] = {0};

    // seed for random variable
    srand(time(NULL));

    // initialize MPI and rank and size
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // initialize status
    MPI_Status status;

    // check if number of processes is equal to P
    if (size != P)
    {
        printf("Number of processes must be equal to P i.e 4\n");
        MPI_Finalize();
        return 1;
    }

    int i, j, k, proc_i, proc_j;

    // initialize D0 matrix on rank 0 process
    if (rank == 0)
    {

        for (int i = 0; i < SUB_SIZE; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (i != j)
                {
                    D0[i][j] = 1.0 + (i + j) / (double)N;
                }
            }
        }
    }

    // distribute D0 matrix to all processes
    MPI_Scatter(&D0[0][0], SUB_SIZE * N, MPI_INT, &sendbuf[0], SUB_SIZE * N, MPI_INT, 0, MPI_COMM_WORLD);

    // compute submtraix of D
    for (k = 0; k < N; k++)
    {
        for (i = 0; i < SUB_SIZE; i++)
        {
            for (j = 0; j < N; j++)
            {
                D[i][j] = fmin(D[i][j], D0[i][k] + D0[k][j]);
            }
        }
    }

    proc_i = rank / SQRT_P;
    proc_j = k / SUB_SIZE;

    // exchange submatrices
    MPI_Sendrecv(&sendbuf[0], SUB_SIZE * N, MPI_INT, (proc_i * SQRT_P + (proc_j + 1) % SQRT_P), 0, &recvbuf[0], SUB_SIZE * N, MPI_INT, (proc_i * SQRT_P + (proc_j - 1 + SQRT_P) % SQRT_P), 0, MPI_COMM_WORLD, &status);

    // copy received submatrix to D0
    for (i = 0; i < SUB_SIZE; i++)
    {
        for (j = 0; j < N; j++)
        {
            D[i][j] = recvbuf[i * N + j];
            printf("D[%d][%d] = %f\n", i, j, D[i][j]);

        }
    }


    // gather submatrices to form final D matric on rank 0 process
    MPI_Gather(&D[0][0], SUB_SIZE * N, MPI_INT, &D0[0][0], SUB_SIZE * N, MPI_INT, 0, MPI_COMM_WORLD);

   
    // finalize MPI
    MPI_Finalize();

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define generate_data(i, j) (i) + (j) * (j)

int main(int argc, char **argv)
{
    int i, j, pid, np, mtag, count;
    double t0, t1;
    int data[100][100], row_sum[100];
    MPI_Status status, sr;
    MPI_Request req_s, req_r;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if (pid == 0)
    { // generate data[]

        // generate first 50 row of the matrix
        for (i = 0; i < 50; i++)
        {
            for (j = 0; j < 100; j++)
            {
                data[i][j] = generate_data(i, j);
            }

            // while data is generated send first 25 rows to process 1

            if (i == 24)
            {
                mtag = 1;
                MPI_Isend(data, 25 * 100, MPI_INT, 1, mtag, MPI_COMM_WORLD, &req_s);
            }
        }

        // send the remaining 25 rows to process 1
        mtag = 2;
        // add offset to data to send the remaining 25 rows
        MPI_Isend(data[25], 25 * 100, MPI_INT, 1, mtag, MPI_COMM_WORLD, &req_r);

        // generate the remaining 50 rows of matrix
        for (i = 50; i < 100; i++)
            for (j = 0; j < 100; j++)
                data[i][j] = generate_data(i, j);

        // calculate row sums for the first 50 rows
        for (i = 50; i < 100; i++)
        {
            row_sum[i] = 0;
            for (j = 0; j < 100; j++)
                row_sum[i] += data[i][j];
            
        }

        // Wait for an MPI request to complete
        MPI_Wait(&req_r, &status);

        /*** receive computed row_sums from pid 1 ***/
        mtag = 3;
        MPI_Recv(row_sum, 50, MPI_INT, 1, mtag, MPI_COMM_WORLD, &status);
        for (i = 0; i < 100; i++)
        {
            // print the row sums
            printf(" %d  \n", row_sum[i]);

            if (i % 5 == 0)
                printf("\n");
        }
    }

    else
    { /*** pid == 1 ***/
        mtag = 1;

        //recieve first 25 rows from pid 0
        MPI_Irecv(data, 25 * 100, MPI_INT, 0, mtag, MPI_COMM_WORLD, &req_r);

        MPI_Wait(&req_r, &sr);

        //compute row sums for the first 25 rows
        for (i = 0; i < 25; i++)
        {

            row_sum[i] = 0;
            for (j = 0; j < 100; j++)
                row_sum[i] += data[i][j];

        }

        //recieve the remaining 25 rows from pid 0
        mtag = 2;
        MPI_Irecv(data[25], 25 * 100, MPI_INT, 0, mtag, MPI_COMM_WORLD, &req_r);

        MPI_Wait(&req_r, &sr);

        //compute row sums for the remaining 25 rows
        for (i = 25; i < 50; i++)
        {

            row_sum[i] = 0;
            for (j = 0; j < 100; j++)
                row_sum[i] += data[i][j];

        }

        /*** Send computed row_sums to pid 0 ***/
        mtag = 3;

        MPI_Send(row_sum, 50, MPI_INT, 0, mtag, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 1;
} /****************** End of function main() ********************/



#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define sgn(x) (((x) < 0.0) ? (-1.0) : (1.0))
#define c1 1.23456
#define c2 6.54321
#define n 123456

// Calculate the force on each particle
void calc_force(double *x, double *f, int rank)
{
    int i, j;
    double diff, tmp;

    // Calculate the chunk size for each rank
    int chunk_size = n / rank;
    int start_index = rank * chunk_size;
    int end_index = (rank + 1) * chunk_size;

    // Get the rank of the current process
    int current_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);

    // If the rank is the last rank, then the end index is n
    if (rank == current_rank - 1)
    {
        end_index = n;
    }

    // Initialize f array with 0.0
    for (i = start_index; i < end_index; i++)
    {
        f[i] = 0.0;
    }

    // for llops to calculate the force between each particle
    for (i = 1; i < n; i++)
    {
        for (j = 0; j < i; j++)
        {
            diff = x[i] - x[j];
            tmp = c1 / (diff * diff * diff) - c2 * sgn(diff) / (diff * diff);

            // If the rank is 0 and j is less than the chunk size, then update f[j] on rank 0
             if (rank == 0 && j < chunk_size)
            {
                // Update f[j] on rank 0
                MPI_Send(&tmp, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
            }
            // If the rank is not 0 and i is less than the chunk size, then receive f[i] from rank 0
            else if (rank != 0 && i < chunk_size)
            {
                // Receive updated f[i] from rank 0
                MPI_Recv(&tmp, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            // If the rank owns the particle, then update f[i]
            if (rank == i / chunk_size)
            {
                // Update f[i] on the rank that owns the particle
                f[i] += tmp;
            }
            // If the rank does not own the particle, then send f[i] to the rank that owns the particle
            else
            {
                // Send updated f[i] to the rank that owns the particle
                MPI_Send(&tmp, 1, MPI_DOUBLE, i / chunk_size, 0, MPI_COMM_WORLD);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    //initialize  variables
    double *x, *f;
    int i, rank;

    // Initialize MPI and get the rank of the current process
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Allocate memory for x and f arrays
    x = (double *)malloc(n * sizeof(double));
    f = (double *)malloc(n * sizeof(double));

    /*
    following two for loops assiggn random value between 0 and n to x array
    to assign random value to x array, we first assign value from 0 to n to x array 
    and then we shuffle the array to get random value between 0 and n
    */

   //assign value from 0 to n to x array
    for (i = 0; i < n; i++)
    {
        x[i] = i;
    }

    //shuffle the array to get random value between 0 and n
    for (int i = n - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);

        // Swap numbers[i] and numbers[j]
        int temp = x[i];
        x[i] = x[j];
        x[j] = temp;
    }

    calc_force(x, f, rank); // Pass the rank to the calc function

    if (rank == 0)
    { // Only rank 0 prints the calculated forces
        for (i = 0; i < n; i++)
        {
            printf("f[%d] = %f\n", i, f[i]);
        }
    }

    // Free the allocated memory
    free(x);
    free(f);

    // Finalize MPI
    MPI_Finalize();

    return 0;
}

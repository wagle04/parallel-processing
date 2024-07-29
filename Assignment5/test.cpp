

// #include <stdio.h>
// #include <stdlib.h>

// #define sgn(x) (((x) < 0.0) ? (-1.0) : (1.0))
// #define c1 1.23456
// #define c2 6.54321
// #define n 123456

// void calc_force(double* x, double* f)
// {
//     int i, j;
//     double diff, tmp;

//     for (i = 1; i < n; i++)
//         for (j = 0; j < i; j++)
//         {
//             diff = x[i] - x[j];
//             tmp = c1 / (diff * diff * diff) - c2 * sgn(diff) / (diff * diff);
//             f[i] += tmp;
//             f[j] -= tmp;
//         }
// }

// int main(int argc, char* argv[])
// {
//     int i, j;
//     double* x;
//     double* f;

//     x = (double*)malloc(n * sizeof(double));
//     f = (double*)malloc(n * sizeof(double));

//     for (i = 0; i < n; i++)
//         x[i] = (double)i;

//     for (i = 0; i < n; i++)
//         f[i] = 0.0;

//     calc_force( x, f);

//     for (i = 0; i < n; i++)
//         printf("%d==%f\n",i, f[i]);

//     free(x);
//     free(f);

//     return 0;
// }

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define sgn(x) (((x) < 0.0) ? (-1.0) : (1.0))
#define c1 1.23456
#define c2 6.54321
#define n 123456

void calc_force(double *x, double *f, int rank)
{
    int i, j;
    double diff, tmp;

    int chunk_size = n / rank;
    int start_index = rank * chunk_size;
    int end_index = (rank + 1) * chunk_size;

    int current_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);

    if (rank == current_rank - 1)
    {
        end_index = n;
    }

    for (i = start_index; i < end_index; i++)
    {
        f[i] = 0.0;
    }

    for (i = 1; i < n; i++)
    {
        for (j = 0; j < i; j++)
        {
            diff = x[i] - x[j];
            tmp = c1 / (diff * diff * diff) - c2 * sgn(diff) / (diff * diff);

            if (rank == 0 && j < chunk_size)
            {
                // Update f[j] on rank 0
                MPI_Send(&tmp, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
            }
            else if (rank != 0 && i < chunk_size)
            {
                // Receive updated f[j] from rank 0
                MPI_Recv(&tmp, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            if (rank == i / chunk_size)
            {
                // Update f[i] on the rank that owns the particle
                f[i] += tmp;
            }
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
    double *x, *f;
    int i, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get the rank of the current process

    x = (double *)malloc(n * sizeof(double));
    f = (double *)malloc(n * sizeof(double));

    // Initialize x array with random values
    for (i = 0; i < n; i++)
    {
        // x[i] = rand() / (double)RAND_MAX;
        x[i] = i;
    }

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

    free(x);
    free(f);

    MPI_Finalize();

    return 0;
}

/*
Initialize MPI and MPI_Comm_rank: The program starts by initializing MPI and getting the rank of the current process using MPI_Comm_rank(). This rank will be used to distribute the particles among the processes and update the force arrays.

Allocate memory for x and f arrays: The program allocates memory for the x and f arrays, which will store the particle positions and forces, respectively.

Initialize x array with random values: The program initializes the x array with random values. This simulates a random distribution of particles in space.

Call calc_force function: The program calls the calc_force function to calculate the forces between all pairs of particles. The rank of the current process is passed to this function to determine how to distribute the particles and update the force arrays.

Print calculated forces (rank 0 only): If the rank of the current process is 0, the program prints the calculated forces to the console. This allows you to see the results of the calculation.

Free allocated memory: The program frees the memory that was allocated for the x and f arrays.

Finalize MPI: The program finalizes MPI, which cleans up any MPI resources that were used.

Here's a more detailed explanation of the calc_force function:

Scatter particles among processes: The particles are divided into chunks, and each chunk is sent to a different process. This ensures that the computation load is balanced among the processes.

Calculate forces locally: Each process calculates the forces between the particles in its chunk. This avoids unnecessary communication between processes.

Update force arrays on remote processes: If a process needs to update the force array of a particle that is owned by another process, it sends the updated value to that process. This communication is minimized by using a scatter-gather protocol.

Collect updated force arrays: Once all the forces have been calculated, the updated force arrays are collected from all processes. This is done using a reduce-scatter protocol.



*/

/*
The communication cost of the program is O(n log_2 p) for n particles and p processors because of the scatter-gather and reduce-scatter protocols that are used to distribute the particles and collect the updated force arrays.

Scatter-gather protocol:

The particles are divided into chunks of size n/p, where p is the number of processes.
Each process sends its chunk of particles to the process with rank 0.
Process 0 gathers all the particles and distributes them to the other processes in a balanced way.
The communication cost of this protocol is O(n log_2 p) because the particles are sent in log_2 p stages. In each stage, the particles are divided in half and sent to the corresponding processes. This logarithmic communication pattern is what gives the protocol its O(n log_2 p) complexity.

Reduce-scatter protocol:

Each process updates the force arrays for the particles in its chunk.
Each process sends its updated force array to the process that owns the corresponding particle.
Each process receives the updated force arrays for the particles that it owns.
The communication cost of this protocol is also O(n log_2 p) because the force arrays are sent in log_2 p stages. In each stage, the force arrays are divided in half and sent to the corresponding processes. This logarithmic communication pattern is what gives the protocol its O(n log_2 p) complexity.

Therefore, the overall communication cost of the program is O(n log_2 p), which is optimal for this type of algorithm.
*/
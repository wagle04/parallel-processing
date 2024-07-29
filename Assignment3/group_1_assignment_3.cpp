

/*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// function to find minimum of two numbers
int min(int a, int b)
{
    return (a < b) ? a : b;
}

// structure to store the least value and its position
struct
{
    int value;
    int pos;
} least;

// function implementation
void HW3(int SOURCE, int n, int **edge, int *distance, MPI_Comm comm)
{
    int i, j, count, tmp, *found;

    // array to keep track of visited nodes
    found = (int *)calloc(n, sizeof(int));

    // calculate rank and size
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    // calculate local size of chunk for each process
    int local_n = n / size;
    int local_start = rank * local_n;
    int local_end = (rank == size - 1) ? n : local_start + local_n;

    // initialization: set initial distances from SOURCE to all nodes
    for (i = 0; i < n; i++)
    {
        found[i] = 0;
        distance[i] = edge[SOURCE][i];
    }
    found[SOURCE] = 1;
    count = 1;

    while (count < n)
    {
        // set least value to a large number and least position to -1
        least.value = 987654321;
        least.pos = -1;

        // find the least value for current process
        for (i = local_start; i < local_end; i++)
        {
            tmp = distance[i];
            if ((!found[i]) && (tmp < least.value))
            {

                least.value = tmp;
                least.pos = i;
            }
        }

        // use MPI_Allreduce to find the least value and its position among all processes
        MPI_Allreduce(&least, &least, 1, MPI_2INT, MPI_MINLOC, comm);

        found[least.pos] = 1;
        count++;

        // update the distance array for all the nodes
        for (i = local_start; i < local_end; i++)
        {
            {
                if (!(found[i]))
                {
                    distance[i] = min(distance[i], least.value + edge[least.pos][i]);
                }
            }
        }
    }

    // free the memory
    free(found);
}

int main(int argc, char *argv[])
{
    int rank;

    // initialize MPI
    MPI_Init(&argc, &argv);

    MPI_Comm comm = MPI_COMM_WORLD;

    // initiliaze graph variables
    int n = 100;
    int **edge, *distance;
    int SOURCE = 0;

    // allocate memory for the graph and distances
    edge = (int **)malloc(sizeof(int *) * n);

    // initialize the graph with random values
    for (int i = 0; i < n; ++i)
    {
        edge[i] = (int *)malloc(sizeof(int) * n);
        for (int j = 0; j < n; ++j)
        {

            if (i == j)
            {
                edge[i][j] = 0;
            }
            else
            {
                edge[i][j] = rand() % 100 + 1;
            }
        }
    }

    distance = (int *)malloc(sizeof(int) * n);

    // call the function
    HW3(SOURCE, n, edge, distance, comm);

    // print the distance array for each process
    for (int i = 0; i < n; i++)
    {
        printf(" distance[%d]: %d\n", i, distance[i]);
    }

    // free the memory
    free(edge);
    free(distance);

    // finalize MPI
    MPI_Finalize();

    return 0;
}
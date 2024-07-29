// /********************************************************************* 
//  Input:  n and edge[n][n], where n is the number of vertices of a graph  
//          edge[i][j] is the length of the edge from vertex i to vertex j 
//  Output: distance[n], the distance from the SOURCE vertex to vertex i. 
//  *********************************************************************/ 

// void HW3(int SOURCE, int n, int **edge, int *distance) { 
//   int i, j, count, tmp, least, leastPos, *found; 
 
//   found = (int *) calloc( n, sizeof(int) ); 
//   for(i=0; i<n; i++) { 
//     found[i]= 0; 
//     distance[i] = edge[SOURCE][i]; 
//   } 
//   found[SOURCE] = 1 ; 
//   count = 1 ; 
 
//   while( count < n ) { 
//     least = 987654321 ; 

//     // parallelize the loop giben below


//     for(i=0; i<n; i++) { // <-- parallelize this loop 
//       tmp = distance[i] ; 
//       if( (!found[i]) && (tmp < least) ) { 
//         least = tmp ; 
//         leastPos = i ; 
//       } 
//     } 

 
//     found[leastPos] = 1; 
//     count++ ; 
//     for(i=0; i<n; i++) { // <-- parallelize this loop 
//       if( !(found[i]) ) 
//          distance[i] = min(distance[i], least+edge[leastPos][i]); 
//     } 
//   } /*** End of while ***/ 
 
//   free(found) ; 
// } 
 
 
 /*********************************************************************************/

//  #include <stdio.h>
// #include <stdlib.h>
// #include <mpi.h>

// int min(int a, int b) {
//     return (a < b) ? a : b;
// }

// void HW3(int SOURCE, int n, int **edge, int *distance) {
//     int i, j, count, tmp, least, leastPos, *found;

//     found = (int *)calloc(n, sizeof(int));

//     int rank, size;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &size);

//     int local_n = n / size;
//     int local_start = rank * local_n; 
//     int local_end = (rank == size - 1) ? n : local_start + local_n;

//     for (i = 0; i < n; i++) {
//         found[i] = 0;
//         distance[i] = edge[SOURCE][i];
//     }
//     found[SOURCE] = 1;
//     count = 1;

//     while (count < n) {
//         least = 987654321;

//         if (local_start <= count && count < local_end) {
//             for (i = local_start; i < local_end; i++) {
//                 tmp = distance[i];
//                 if ((!found[i]) && (tmp < least)) {
                   
//                         least = tmp;
//                         leastPos = i;
//                                 }
//             }
//         }

//         MPI_Allreduce(MPI_IN_PLACE, &least, 1, MPI_INT, 0, MPI_COMM_WORLD);
//         MPI_Bcast(&leastPos, 1, MPI_INT, 0, MPI_COMM_WORLD);

       
//             found[leastPos] = 1;
//             count++;

//         MPI_Bcast(&count, 1, MPI_INT, 0, MPI_COMM_WORLD);

//         for (i = 0; i < n; i++) {
//             if (!(found[i])) {
//                 distance[i] = min(distance[i], least + edge[leastPos][i]);
//             }
//         }
//     } /*** End of while ***/

//     free(found);
// }


 /*********************************************************************************/


// #include <stdio.h>
// #include <stdlib.h>
// #include <mpi.h>

// void HW3(int SOURCE, int n, int **edge, int *distance) {

//     int i, j, count, tmp, least, leastPos, *found;

//     found = (int *) calloc(n, sizeof(int));

//     for (i = 0; i < n; i++) {
//         found[i] = 0;
//         distance[i] = edge[SOURCE][i];
//     }

//     found[SOURCE] = 1;
//     count = 1;


//     while (count < n) {
//         least = 987654321;

        
//         MPI_Init(NULL, NULL);
//         int rank, size;
//         MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//         MPI_Comm_size(MPI_COMM_WORLD, &size);
        
//         int local_least = 987654321;
//         int local_leastPos;
        
//         for (i = rank; i < n; i += size) {
//             tmp = distance[i];
//             if ((!found[i]) && (tmp < local_least)) {
//                 local_least = tmp;
//                 local_leastPos = i;
//             }
//         }
        
//         MPI_Allreduce(&local_least, &least, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
//         MPI_Allreduce(&local_leastPos, &leastPos, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
        
//         MPI_Barrier(MPI_COMM_WORLD);
//         MPI_Finalize();

//         found[leastPos] = 1;
//         count++;

//         MPI_Init(NULL, NULL);
//         for (i = rank; i < n; i += size) {
//             if (!(found[i])) {
//                 int local_min = distance[i];
//                 for (j = 0; j < n; j++) {
//                     if (!found[j]) {
//                         local_min = min(local_min, least + edge[leastPos][j]);
//                     }
//                 }
//                 distance[i] = local_min;
//             }
//         }
        
//         MPI_Barrier(MPI_COMM_WORLD);
//         MPI_Finalize();
//     } /*** End of while ***/
//     free(found);
// }


 /*********************************************************************************/



// #include <stdio.h>
// #include <stdlib.h>
// #include <mpi.h>

// void HW3(int SOURCE, int n, int **edge, int *distance) {
//   int i, j, count, tmp, least, leastPos, *found, localLeast, localLeastPos;
//   int rank, size;

//   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//   MPI_Comm_size(MPI_COMM_WORLD, &size);

//   found = (int *)calloc(n, sizeof(int));
//   for (i = 0; i < n; i++) {
//     found[i] = 0;
//     distance[i] = edge[SOURCE][i];
//   }
//   found[SOURCE] = 1;
//   count = 1;

//   while (count < n) {
//     least = 987654321;

//     // Parallelize the first loop
//     for (i = rank; i < n; i += size) {
//       if ((!found[i]) && (distance[i] < least)) {
//         least = distance[i];
//         leastPos = i;
//       }
//     }

//     // Find the global minimum using MPI_Allreduce
//     MPI_Allreduce(&least, &localLeast, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
//     if (least == localLeast) {
//       MPI_Allreduce(&leastPos, &localLeastPos, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
//     } else {
//       localLeastPos = n;  // Set to a value outside the valid range
//     }

//     // Broadcast the position of the local minimum to all processes
//     MPI_Bcast(&localLeastPos, 1, MPI_INT, 0, MPI_COMM_WORLD);

//     if (localLeastPos < n) {
//       if (!found[localLeastPos]) {
//         found[localLeastPos] = 1;
//         count++;
//       }

//       // Parallelize the second loop
//       for (i = rank; i < n; i += size) {
//         if (!(found[i])) {
//           distance[i] = distance[i] < localLeast + edge[localLeastPos][i] ? distance[i] : localLeast + edge[localLeastPos][i];
//         }
//       }
//     }
//   } /*** End of while ***/

//   free(found);
// }

// int main(int argc, char *argv[]) {
//   int rank;

//   MPI_Init(&argc, &argv);
//   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

//   // Define the graph and other necessary variables here

//   HW3(SOURCE, n, edge, distance);

//   MPI_Finalize();
//   return 0;
// }


#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Function to find the minimum of two integers
int min(int a, int b)
{
    return (a < b) ? a : b;
}

struct
{
    int value;
    int pos;
} least;

// Parallel implementation of Dijkstra's algorithm
void HW3(int SOURCE, int n, int **edge, int *distance, MPI_Comm comm)
{
    int i, j, count, tmp, *found;

    // Array to keep track of visited nodes
    found = (int *)calloc(n, sizeof(int));

    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    // Calculate local portion of the graph for each process
    int local_n = n / size;
    int local_start = rank * local_n;
    int local_end = (rank == size - 1) ? n : local_start + local_n;

    // Initialization: set initial distances from SOURCE to all nodes
    for (i = 0; i < n; i++)
    {
        found[i] = 0;
        distance[i] = edge[SOURCE][i];
    }
    found[SOURCE] = 1;
    count = 1;

    // Dijkstra's algorithm loop
    while (count < n)
    {
        least.value = 987654321;
        least.pos = -1;

        // Find the node with the smallest tentative distance among the local nodes
        for (i = local_start; i < local_end; i++)
        {
            tmp = distance[i];
            if ((!found[i]) && (tmp < least.value))
            {
                least.value = tmp;
                least.pos = i;
            }
        }

        // Find the global node with the smallest tentative distance
        MPI_Allreduce(&least, &least, 1, MPI_2INT, MPI_MINLOC, comm);

        // Mark the found node as visited
        found[least.pos] = 1;
        count++;

        // Update distances to neighboring nodes in the local portion of the graph
        for (i = local_start; i < local_end; i++)
        {
            if (!(found[i]))
            {
                distance[i] = min(distance[i], least.value + edge[least.pos][i]);
            }
        }
    }

    free(found);
}

int main(int argc, char *argv[])
{
    int rank;

    MPI_Init(&argc, &argv);

    MPI_Comm comm = MPI_COMM_WORLD;

    int n = 100;
    int **edge, *distance;
    int SOURCE = 0;

    // Allocate memory for the graph and distances
    edge = (int **)malloc(sizeof(int *) * n);
    for (int i = 0; i < n; ++i)
    {
        edge[i] = (int *)malloc(sizeof(int) * n);
        for (int j = 0; j < n; ++j)
        {
            // Randomly initialize edge weights (for demonstration purposes)
            edge[i][j] = rand() % 100 + 1;
        }
    }

    distance = (int *)malloc(sizeof(int) * n);

    // Run parallel Dijkstra's algorithm
    HW3(SOURCE, n, edge, distance, comm);

    // Print the calculated distances from the SOURCE node
    for (int i = 0; i < n; i++)
    {
        printf(" distance[%d]: %d\n", i, distance[i]);
    }

    // Free allocated memory and finalize MPI
    free(edge);
    free(distance);

    MPI_Finalize();

    return 0;
}



#include <stdio.h>
#include <mpi.h>
#include <math.h>


//function prototype
void mybarrier(MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    //calcualte the  height of binary tree (log2 of the number of processes)
    int height = (int)(log2(size));

    //iterate throught binary tree height
    for (int step = 0; step < height; step++) {

        //calculate the partner process using XOR operation with the steps bit
        int destination = rank ^ (1 << step);

        //determine the direction of comunicatoin based on rank comparison
        if (rank < destination) {

            // if curernt rank of process is less than destination, send the completion message 
            MPI_Send(NULL, 0, MPI_INT, destination, 0, comm);
            printf("send operation between %d and %d\n", rank, destination);
        }
        else {
            // if current rank is greater than destination, receive the completion message
            MPI_Recv(NULL, 0, MPI_INT, destination, 0, comm, MPI_STATUS_IGNORE);
            printf("recieve operation between %d and %d\n", rank, destination);
        }
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;

    int rank;
    MPI_Comm_rank(comm, &rank);

    // perform some computation before calling barrier
    printf("Process %d is performing computtion before the barrier....\n", rank);

    // call the barrier function
    mybarrier(comm);

    // perform some computation after calling barrier
    printf("Process %d is performing computtion after the barrier....\n", rank);

    // continue with the rest of the code

    MPI_Finalize();
    return 0;
}

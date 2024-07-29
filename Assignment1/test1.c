#include <mpi.h> 

#include <stdio.h> 

 

void main(int argc, char **argv) { 

   int i ; 

   char message[20] ; 

   int rank ; //This is process id 

   int np ; //number of processes 

   MPI_Status status ; 

 

   MPI_Init(&argc, &argv) ; 

 

   MPI_Comm_rank(MPI_COMM_WORLD, &rank); 

   MPI_Comm_size(MPI_COMM_WORLD, &np) ; 

 

   if(rank==0) { 

      strcpy(message, "Hello.") ; 

      MPI_Send(message, strlen(message), MPI_CHAR, 1, 999, MPI_COMM_WORLD) ; 

   } 

   else { 

         MPI_Recv(message, 20, MPI_CHAR, 0, 999, MPI_COMM_WORLD, &status) ; 

         printf("Received: %s\n", message) ; 

   } 

 

   MPI_Finalize(); 

} 
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#define MASTER 0
#define SIZE 128

int main (int argc, char** argv){

    int i, j;
    int my_rank;
    int my_size = 10;
    int source;
    int dest;
    int tag = 0;
    char message[SIZE];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &my_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    for (i = 0; i < my_size; i++) {

        dest = (my_rank+1) % my_size;
        source = (my_rank-1) % my_size ;


        if (i == my_size - 1){
            sprintf(message, "Hello from %d, to %d", source, 0);
            MPI_Send(message, strlen(message)+1, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
        } else {
            sprintf(message, "Hello from %d, to %d", source, dest);
            MPI_Send(message, strlen(message)+1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
        }


        if (my_rank == 0){
            sprintf(message, "Hello from %d, to %d", 0, dest);
            MPI_Recv(message, SIZE, MPI_CHAR, my_size -1, tag, MPI_COMM_WORLD, &status);
        } else {
            sprintf(message, "Hello from %d, to %d", source, dest);
            MPI_Recv(message, SIZE, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
        }
        printf("%s\n", message);
    }

    MPI_Finalize();
    return 0;
}

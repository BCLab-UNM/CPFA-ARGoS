#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int myrank, nprocs;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

char hostname[1024];
hostname[1023] = '\0';
gethostname(hostname, 1023);

 printf("Hello from processor %d of %d on host %s\n", myrank, nprocs,  hostname);

    MPI_Finalize();
    return 0;
}

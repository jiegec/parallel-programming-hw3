#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

const int MAX_P = 1024;

int data[MAX_P * 10];
int prefix[MAX_P * 10];

int main() {
    int comm_sz, my_rank;
    MPI_Comm comm;

    MPI_Init(NULL, NULL);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &my_rank);

    srand(my_rank + time(NULL));

    int local_prefix[10];
    int local_data[10];
    for (int i = 0;i < 10;i++) {
        local_data[i] = rand() % 10;
        local_prefix[i] = local_data[i];
        if (i > 0) {
            local_prefix[i] += local_prefix[i-1];
        }
    }

    int prefix_before;
    MPI_Scan(&local_prefix[10-1], &prefix_before, 1, MPI_INT, MPI_SUM, comm);
    prefix_before -= local_prefix[10-1];

    for (int i = 0;i < 10;i++) {
        local_prefix[i] += prefix_before;
    }

    MPI_Gather(local_prefix, 10, MPI_INT, prefix, 10, MPI_INT, 0, comm);
    MPI_Gather(local_data, 10, MPI_INT, data, 10, MPI_INT, 0, comm);

    if (my_rank == 0) {
        printf("Data: ");
        for (int i = 0;i < 10 * comm_sz;i++) {
            printf("%d ", data[i]);
        }
        printf("\n");
        printf("Prefix: ");
        for (int i = 0;i < 10 * comm_sz;i++) {
            printf("%d ", prefix[i]);
        }
        printf("\n");
        printf("Expected: ");
        int temp = 0;
        for (int i = 0;i < 10 * comm_sz;i++) {
            temp += data[i];
            printf("%d ", temp);
        }
    }
    MPI_Finalize();
}
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int primo(long int n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    long int max = sqrt(n) + 1;
    for (long int i = 3; i <= max; i += 2)
        if (n % i == 0) return 0;
    return 1;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    long int n;
    if (argc < 2) {
        if (rank == 0) printf("Uso: %s <numero_maximo>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }
    n = strtol(argv[1], NULL, 10);

    double start_time = MPI_Wtime();
    int local_count = 0, total = 0;
    MPI_Request *requests;
    MPI_Status *statuses;

    if (rank == 0) {
        total = (n >= 2) ? 1 : 0;
        int *recv_counts = malloc((num_procs - 1) * sizeof(int));
        requests = malloc((num_procs - 1) * sizeof(MPI_Request));
        statuses = malloc((num_procs - 1) * sizeof(MPI_Status));
        
        for (int i = 1; i < num_procs; i++) {
            int *recv_counts = malloc((num_procs - 1) * sizeof(int));
            for (int i = 1; i < num_procs; i++) {
                MPI_Irecv(&recv_counts[i-1], 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &requests[i-1]);
            }
        }
        MPI_Waitall(num_procs - 1, requests, statuses);
        for (int i = 0; i < num_procs - 1; i++) {
            total += recv_counts[i];
        }
        free(recv_counts);
    }

    if (rank == 0) {
        MPI_Waitall(num_procs - 1, requests, statuses);
        double elapsed = MPI_Wtime() - start_time;
        printf("Primos encontrados: %d\n", total);
        printf("Tempo paralelo: %.4f segundos\n", elapsed);
        free(requests);
        free(statuses);
    }

    MPI_Finalize();
    return 0;
}

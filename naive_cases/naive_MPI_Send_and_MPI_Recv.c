#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int primo(long int n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    long int max = sqrt(n) + 1;
    for (long int i = 3; i <= max; i += 2) {
        if (n % i == 0) return 0;
    }
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
    MPI_Status status;

    if (rank == 0) {
        total = (n >= 2) ? 1 : 0;
    } else {
        long int numbers = (n >= 3) ? ((n - 1) / 2) : 0;
        long int chunk = (num_procs > 1) ? numbers / (num_procs - 1) : 0;
        long int start = 3 + (rank - 1) * chunk * 2;
        long int end = start + (chunk - 1) * 2;
        if (chunk == 0 || start > n) start = n + 1;
        if (end > n) end = n;
        for (long int i = start; i <= end; i += 2) {
            if (primo(i)) local_count++;
        }
        MPI_Send(&local_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        for (int i = 1; i < num_procs; i++) {
            int temp;
            MPI_Recv(&temp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            total += temp;
        }
        double elapsed = MPI_Wtime() - start_time;
        printf("Primos encontrados: %d\n", total);
        printf("Tempo paralelo: %.4f segundos\n", elapsed);
    }

    MPI_Finalize();
    return 0;
}

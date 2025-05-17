#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

int is_prime (long int n) {
    int i;
    for (i = 3; i < (int)(sqrt(n) + 1); i += 2) {
        if (n%i == 0) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    double t_start, t_end;
    int count = 0, total = 0;
    long int i, n;
    int my_rank, n_procs, start, step;

    if (argc < 2) {
        printf("Valor inválido! Entre com um valor do maior inteiro\n");
        return 0;
    } else {
        n = strtol(argv[1], (char **) NULL, 10);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
    t_start = MPI_Wtime();

    start = 3 + my_rank * 2;
    step = n_procs * 2;

    for (i = start; i <= n; i += step)
    {
        if(is_prime(i) == 1) count++;
    }

    if (my_rank == 0){
        total = count;
        for (int origin = 1; origin < n_procs; origin++) {
            MPI_Recv(&count, 1, MPI_INT, origin, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total += count;
        }
    }
    else {
        int bsize;
        MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &bsize);
        bsize += MPI_BSEND_OVERHEAD;
        void *buf = malloc(bsize);
        MPI_Buffer_attach(buf, bsize);

        MPI_Bsend(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        void *tmpbuf;
        int tmpsize;
        MPI_Buffer_detach(&tmpbuf, &tmpsize);
        free(buf);
    }

    t_end = MPI_Wtime();

    if (my_rank == 0) {
        total += 1;    /* Acrescenta o dois, que também é primo */
        printf("Quant. de primos entre 1 e n: %d \n", total);
        printf("Tempo de execucao: %1.3f \n", t_end - t_start);
    }

    MPI_Finalize();
    return(0);
}
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

#define TAMANHO 500000

int primo(int n) {
    int i;
    for (i = 3; i <= (int)(sqrt(n)); i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    double t_inicial, t_final;
    int cont = 0, total = 0;
    int i, n;
    int meu_ranque, num_procs;
    int inicio, dest, raiz = 0, tag = 1;
    MPI_Status estado;

    if (argc < 2) {
        printf("Entre com o valor do maior inteiro como parâmetro.\n");
        return 0;
    } else {
        n = strtol(argv[1], (char **)NULL, 10);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (num_procs < 2) {
        printf("Este programa precisa de no mínimo dois processos.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    if (meu_ranque == 0) {
        // Processo mestre
        inicio = 3;
        int processos_ativos = num_procs - 1;
        t_inicial = MPI_Wtime();

        while (processos_ativos > 0) {
            // Espera um sinal de que um escravo está pronto
            MPI_Recv(NULL, 0, MPI_BYTE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &estado);
            dest = estado.MPI_SOURCE;

            if (inicio < n) {
                // Envia trabalho: somente agora sabemos que o receptor já postou MPI_Recv
                MPI_Rsend(&inicio, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
                inicio += TAMANHO;
            } else {
                // Envia fim
                int fim = 0;
                MPI_Rsend(&fim, 1, MPI_INT, dest, 99, MPI_COMM_WORLD);
                processos_ativos--;
                continue; // não espera resultado
            }

            // Recebe resultado do processo escravo
            MPI_Recv(&cont, 1, MPI_INT, dest, MPI_ANY_TAG, MPI_COMM_WORLD, &estado);
            total += cont;
        }

        total += 1; // inclui o número 2, que é primo
        t_final = MPI_Wtime();

        printf("Quant. de primos entre 1 e %d: %d\n", n, total);
        printf("Tempo de execucao: %1.3f segundos\n", t_final - t_inicial);
    } else {
        // Processos escravos
        while (1) {
            // Handshake: informa que está pronto
            MPI_Send(NULL, 0, MPI_BYTE, raiz, 0, MPI_COMM_WORLD);

            // Espera tarefa
            MPI_Recv(&inicio, 1, MPI_INT, raiz, MPI_ANY_TAG, MPI_COMM_WORLD, &estado);
            if (estado.MPI_TAG == 99) break;

            // Processa intervalo
            cont = 0;
            for (i = inicio; i < inicio + TAMANHO && i < n; i += 2) {
                if (primo(i)) cont++;
            }

            // Envia resultado
            MPI_Rsend(&cont, 1, MPI_INT, raiz, tag, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
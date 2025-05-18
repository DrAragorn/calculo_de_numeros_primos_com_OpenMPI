#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>
#define TAMANHO 500000

int primo (int n) {
    int i;
    for (i = 3; i < (int)(sqrt(n) + 1); i+=2) {
        if(n%i == 0) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) { /* mpi_primosbag.c  */
    double t_inicial, t_final;
    int cont = 0, total = 0;
    int i, n;
    int meu_ranque, num_procs, inicio, dest, raiz=0, tag=1, stop=0;
    MPI_Status estado;
    /* Verifica o número de argumentos passados */
    if (argc < 2) {
        printf("Entre com o valor do maior inteiro como parâmetro para o programa.\n");
        return 0;
    } else {
        n = strtol(argv[1], (char **) NULL, 10);
    }
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    /* Se houver menos que dois processos aborta */
    if (num_procs < 2) {
        printf("Este programa deve ser executado com no mínimo dois processos.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
        return(1);
    }
    /* Registra o tempo inicial de execução do programa */
    t_inicial = MPI_Wtime();
    /* Envia pedaços com TAMANHO números para cada processo */
    if (meu_ranque == 0) {
        inicio = 3;
        int enviados = 0;
        int n_escravos = num_procs - 1;

        int *conts = malloc(n_escravos * sizeof(int));     // buffers para cada escravo
        MPI_Request *requests = malloc(n_escravos * sizeof(MPI_Request));
        MPI_Status status;

        // Envia um primeiro bloco ou aviso de término a cada escravo
        for (dest = 1; dest < num_procs; dest++) {
            if (inicio < n) {
                MPI_Send(&inicio, 1, MPI_INT, dest, 1, MPI_COMM_WORLD); // trabalho
                inicio += TAMANHO;
                enviados++;
            } else {
                int fim = 0;
                MPI_Send(&fim, 1, MPI_INT, dest, 99, MPI_COMM_WORLD); // aviso de término
            }
        }

        // Inicia MPI_Irecv pendentes para todos escravos que receberam trabalho
        for (int i = 0; i < n_escravos; i++) {
            conts[i] = 0;
            MPI_Irecv(&conts[i], 1, MPI_INT, i + 1, MPI_ANY_TAG, MPI_COMM_WORLD, &requests[i]);
        }

        // Processa respostas e continua distribuindo enquanto houver trabalho
        while (enviados > 0) {
            int index;
            MPI_Waitany(n_escravos, requests, &index, &status); // Espera qualquer um terminar
            int escravo = index + 1; // rank do escravo que enviou a mensagem
            int cont = conts[index];  // quantidade de primos recebida

            // Atualiza total
            total += cont;

            // Envia mais trabalho ou fim para esse escravo
            if (inicio < n) {
                MPI_Send(&inicio, 1, MPI_INT, escravo, 1, MPI_COMM_WORLD);
                inicio += TAMANHO;

                // Reinicia Irecv para o próximo resultado desse escravo
                MPI_Irecv(&conts[index], 1, MPI_INT, escravo, MPI_ANY_TAG, MPI_COMM_WORLD, &requests[index]);
            } else {
                int fim = 0;
                MPI_Send(&fim, 1, MPI_INT, escravo, 99, MPI_COMM_WORLD);
                enviados--;
                // Não reinicia Irecv para esse escravo pois acabou
            }
        }

        // free(conts);
        // free(requests);
    }
    else {
        /* Cada processo escravo recebe o início do espaço de busca */
        while (1) {
            MPI_Request req;
            MPI_Irecv(&inicio, 1, MPI_INT, raiz, MPI_ANY_TAG, MPI_COMM_WORLD, &req);
            MPI_Wait(&req, &estado);

            // Se Tag 99, sai do loop; Se não, faz as tarefas;
            if (estado.MPI_TAG == 99)
                break;

            for (i = inicio, cont=0; i < (inicio + TAMANHO) && i < n; i+=2)
                if (primo(i) == 1)
                    cont++;
            /* Envia a contagem parcial para o processo mestre */
            MPI_Send(&cont, 1, MPI_INT, raiz, tag, MPI_COMM_WORLD);
        }
        /* Registra o tempo final de execução */
        t_final = MPI_Wtime();
    }
    if (meu_ranque == 0) {
        t_final = MPI_Wtime();
        total += 1;    /* Acrescenta o 2, que é primo */
        printf("Quant. de primos entre 1 e %d: %d \n", n, total);
        printf("Tempo de execucao: %1.3f \n", t_final - t_inicial);
    }
    /* Finaliza o programa */
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return(0);
}
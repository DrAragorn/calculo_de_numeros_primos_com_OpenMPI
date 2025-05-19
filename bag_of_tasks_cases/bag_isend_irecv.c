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

        // Buffers de Envio        
        MPI_Request* requests_envio = malloc((num_procs - 1) * sizeof(MPI_Request));
        int* buffer_envio = malloc((num_procs - 1) * sizeof(int));

        // Buffers de recepção
        int *conts = malloc(n_escravos * sizeof(int));     // buffers para cada escravo
        MPI_Request *requests_recepcao = malloc(n_escravos * sizeof(MPI_Request));
        MPI_Status status;

        // Envia um primeiro bloco ou aviso de término a cada escravo
        for (dest = 1; dest < num_procs; dest++) {
            int idx = dest - 1;
            if (inicio < n) {
                /*
                    É necessário utilizar o buffer, pois, caso o valor de início seja modificado na iteração i+1,
                    antes do Isend da iteração i ter sido enviado, será enviado para o processo o valor calculado 
                    em i+1 ao invés de i;
                */ 
                buffer_envio[idx] = inicio;  // copia do valor para cada envio
                MPI_Isend(&buffer_envio[idx], 1, MPI_INT, dest, 1, MPI_COMM_WORLD, &requests_envio[idx]);
                inicio += TAMANHO;
                enviados++;
            } else {
                buffer_envio[idx] = 0;
                MPI_Isend(&buffer_envio[idx], 1, MPI_INT, dest, 99, MPI_COMM_WORLD, &requests_envio[idx]);
            }
        }                

        // Inicia MPI_Irecv pendentes para todos escravos que receberam trabalho
        for (int i = 0; i < n_escravos; i++) {
            conts[i] = 0;
            MPI_Irecv(&conts[i], 1, MPI_INT, i + 1, MPI_ANY_TAG, MPI_COMM_WORLD, &requests_recepcao[i]);
        }

        // Processa respostas e continua distribuindo enquanto houver trabalho
        while (enviados > 0) {
            int index;
            MPI_Waitany(n_escravos, requests_recepcao, &index, &status); // Espera qualquer um terminar

            // if (index == MPI_UNDEFINED) {
            //     fprintf(stderr, "MPI_Waitany retornou MPI_UNDEFINED. Isso indica que todos os Irecv já completaram.\n");
            //     break;
            // }

            int escravo = index + 1;
            int cont = conts[index];  // quantidade de primos recebida
            total += cont;

            if (inicio < n) {
                buffer_envio[index] = inicio;
                MPI_Isend(&buffer_envio[index], 1, MPI_INT, escravo, 1, MPI_COMM_WORLD, &requests_envio[index]);
                inicio += TAMANHO;

                // Posta novo Irecv para escutar a próxima resposta desse escravo
                MPI_Irecv(&conts[index], 1, MPI_INT, escravo, MPI_ANY_TAG, MPI_COMM_WORLD, &requests_recepcao[index]);
            } else {
                buffer_envio[index] = 0;
                MPI_Isend(&buffer_envio[index], 1, MPI_INT, escravo, 99, MPI_COMM_WORLD, &requests_envio[index]);

                enviados--; // este escravo foi encerrado, não fazemos mais Irecv para ele
            }
        }

        free(buffer_envio);
        free(requests_envio);
        free(requests_recepcao);
        free(conts);
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
            MPI_Request request;
            MPI_Isend(&cont, 1, MPI_INT, raiz, tag, MPI_COMM_WORLD, &request);
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
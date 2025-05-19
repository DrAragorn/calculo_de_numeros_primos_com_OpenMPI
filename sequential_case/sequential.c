#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

int primo(long int n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    
    long int max = (long int)sqrt(n) + 1;
    for (long int i = 3; i < max; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <numero_maximo>\n", argv[0]);
        return 1;
    }

    long int n = strtol(argv[1], NULL, 10);
    
    struct timeval inicio, fim;
    gettimeofday(&inicio, NULL);
    
    int contagem = 0;
    if (n >= 2) contagem = 1; // Conta o número 2
    
    for (long int i = 3; i <= n; i += 2) {
        if (primo(i)) {
            contagem++;
        }
    }

    gettimeofday(&fim, NULL);
    double tempo = (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec) / 1000000.0;

    printf("Quantidade de primos até %ld: %d\n", n, contagem);
    printf("Tempo de execução: %.3f segundos\n", tempo);

    return 0;
}

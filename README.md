# Cálculo de Números Primos com OpenMPI

Projeto para calcular quais os números primos de 3 até n.

Trabalho para a disciplina Laboratório de Programação Paralela (TCC00344), ministrada para professora Dra. Lucia Drummond.

Linguagem de programação usada: C.

Tecnologias: OpenMPI.



Descrição do trabalho:

    Execute os dois programas de Cálculo de Números Primos (Naive e Bag of Tasks), usando as seguintes instruções de comunicação de envio ponto-a-ponto:

    MPI_Send
    MPI_Isend
    MPI_Rsend
    MPI_Bsend
    MPI_Ssend

    Combinando com as operações de  MPI_Recv ou MPI_Irecv

    Para o caso Naive -  10 combinações
    Para  o caso Bag of tasks - 9 combinações


    Avalias os speedups e eficiencias dos 19 casos.


    Fazer um relatorio e slides para apresentacao em 15 minutos. Poste os programas, o relatorio e os slides. Trabalho feito em grupo de 3 pessoas. Uma delas vai ser sorteada para  apresentacao. A nota do grupo inteiro  sera correspondente a dessa apresentacao.



Avisos da professora:

    * Usar o mpi_primos.c do repositório das aulas;
    * Trocar o MPI_Reduce por "um monte" de send e receive:
    * Corrigir o código mpi_primos.c, porque ele está originalmente com bugs:
    * Otimizar o código no geral;
    * Para cálculo dos primos:
        - Pular números pares, porque não tem como serem primos;
        - Dividir o grupo de números ímpares de uma sequência para uma thread testar se há números primos, a sequência seguinte irá para outro thread testar, e assim em diante até o final.
    * Corrigir erro de quando não há trabalho para todas as threads.


Documentação: https://drive.google.com/drive/folders/1mvYeigoUfU_j5NoQ0EBqGK-It9VbMfO8?usp=sharing


Tarefas a fazer:

* Desenvolver os programas:
    - Programas com _naive_:
        * MPI_Send + MPI_Recv; [✓]
        * MPI_Send + MPI_Irecv; [✓]
        * MPI_Isend + MPI_Recv; [✓]
        * MPI_Isend + MPI_Irecv; [✓]
        * MPI_Rsend + MPI_Recv; [✓]
        * MPI_Rsend + MPI_Irecv; [✓]
        * MPI_Bsend + MPI_Recv; [✓]
        * MPI_Bsend + MPI_Irecv; [✓]
        * MPI_Ssend + MPI_Recv; [✓]
        * MPI_Ssend + MPI_Irecv; [✓]
    - Programas com _bag os tasks_:
        * MPI_Send + MPI_Recv; [✓]
        * MPI_Send + MPI_Irecv; [✓]
        * MPI_Isend + MPI_Recv; [✓]
        * MPI_Isend + MPI_Irecv; [✓]
        * MPI_Rsend + MPI_Recv;
        * MPI_Bsend + MPI_Recv;
        * MPI_Bsend + MPI_Irecv;
        * MPI_Ssend + MPI_Recv; [✓]
        * MPI_Ssend + MPI_Irecv; [✓]
* Realizar os 19 casos testes, e coletar informações;[Thiago - fazendo]
* Escrever relatório:
    - Introdução;
    - Metodologia;
    - Desenvolvimento:
        * Casos _naive_;
        * Casos _bag of tasks_.
    - Conclusão.
* Montar slides da apresentação;
* Estar todos membros do grupo cientes de tudo do trabalho, para caso seja sorteado.
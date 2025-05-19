#!/bin/bash

N=10000000
NUM_PROCS=4
EXECS_DIR="execs"
RESULTS_FILE="resultados.csv"

echo "Programa,Tempo_Sequencial(s),Tempo_Paralelo(s),Speedup,Eficiência" > $RESULTS_FILE

run_test() {
    local exec=$1
    local np=$2
    local output=$(mpirun -np $np $exec $N 2>&1)
    
    local tempo=$(echo "$output" | grep -E "Tempo de execucao:|Tempo de execução:" | grep -oE "[0-9]+\.[0-9]+")
    
    if [ -z "$tempo" ]; then
        echo "0.000"
    else
        echo "$tempo"
    fi
}

for exec in $EXECS_DIR/*; do
    exe_name=$(basename $exec)
    
    echo "Testando $exe_name..."
    
    t_seq=$(run_test $exec 1)

    t_par=$(run_test $exec $NUM_PROCS)
    
    if [ $(echo "$t_par == 0" | bc) -eq 1 ]; then
        speedup="N/A"
        efficiency="N/A"
    else
        speedup=$(echo "scale=2; $t_seq / $t_par" | bc)
        efficiency=$(echo "scale=2; $speedup / $NUM_PROCS" | bc)
    fi

    echo "$exe_name,$t_seq,$t_par,$speedup,$efficiency" >> $RESULTS_FILE
done

echo -e "\nResultados finais:"
column -t -s',' $RESULTS_FILE

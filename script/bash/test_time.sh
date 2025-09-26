#!/bin/bash
GRID="1000"
ITERATIONS="600"
SEEDX="500"
SEEDY="500"
SEEDZ="500"
S=1
E=10
for p in "100" "200" 
do 
   
    PARTICLES="$p"
    echo "Particle ${PARTICLES} ${GRID} ${ITERATIONS}" >> test_times.txt
    PARAM="-g ${GRID} -n ${PARTICLES} -i ${ITERATIONS} -s ${SEEDX} -s ${SEEDY}"
    FILE="./sequential/main_linear.o"
    CMD="../../"$FILE" ${PARAM} "
    echo  $FILE >> test_times.txt
    echo $CMD
    for (( c=$S; c<=$E; c++ ))
    do
        $CMD >> test_times.txt
    done

    for T in 2 4 8 16 32 
    do 
        FILE="./pthread/main_linear_partial.o"
        CMD="../../"$FILE" ${PARAM} -t ${T}"
        echo $FILE $T >> test_times.txt
        echo $CMD
        for (( c=$S; c<=$E; c++ ))
        do
            $CMD >> test_times.txt
        done
    done 
    for T in 2 4 8 16 32 
    do 
        FILE="./omp/main_linear_partial.o"
        CMD="../../"$FILE" ${PARAM} -t ${T}"
        echo $FILE $T >> test_times.txt
        echo $CMD
        for (( c=$S; c<=$E; c++ ))
        do
            $CMD >> test_times.txt
        done
    done 
done

for g in 200 400 
do 
    GRID="$g"
    PARTICLES="$(($g*$g*7/10))"
    SEEDX="$(($g/2))"
    SEEDY="$(($g/2))"
    SEEDZ="$(($g/2))"
    echo "Grid ${PARTICLES} ${GRID} ${ITERATIONS} ${SEEDX} ${SEEDY}" >> test_times.txt
    PARAM="-g ${GRID} -n ${PARTICLES} -i ${ITERATIONS} -s ${SEEDX} -s ${SEEDY}"
    FILE="./sequential/main_linear.o"
    CMD="../../"$FILE" ${PARAM} "
    echo  $FILE >> test_times.txt
    for (( c=$S; c<=$E; c++ ))
    do
        $CMD >> test_times.txt
    done

    for T in 2 4 8 16 32 
    do 
        FILE="./pthread/main_linear_partial.o"
        CMD="../../"$FILE" ${PARAM} -t ${T}"
        echo $FILE $T >> test_times.txt
        echo $CMD
        for (( c=$S; c<=$E; c++ ))
        do
            $CMD >> test_times.txt
        done
    done  
    for T in 2 4 8 16 32 
    do 
        FILE="./omp/main_linear_partial.o"
        CMD="../../"$FILE" ${PARAM} -t ${T}"
        echo $FILE $T >> test_times.txt
        echo $CMD
        for (( c=$S; c<=$E; c++ ))
        do
            $CMD >> test_times.txt
        done
    done 
done
GRID="1000"
PARTICLES="700000"
SEEDX="500"
SEEDY="500"
SEEDZ="500"
for i in "200" "400"
do 
    ITERATIONS="$i"
    echo "Iterations ${PARTICLES} ${GRID} ${ITERATIONS} ${SEEDX} ${SEEDY}" >> test_times.txt
    PARAM="-g ${GRID} -n ${PARTICLES} -i ${ITERATIONS} -s ${SEEDX} -s ${SEEDY}"
    FILE="./sequential/main_linear.o"
    CMD="../../"$FILE" ${PARAM} "
    echo  $FILE >> test_times.txt
    echo $CMD
    for (( c=$S; c<=$E; c++ ))
    do
        $CMD >> test_times.txt
    done

    for T in 2 4 8 16 32 
    do 
        
        FILE="./pthread/main_linear_partial.o"
        CMD="../../"$FILE" ${PARAM} -t ${T}"
        echo $FILE $T >> test_times.txt
        echo $CMD
        for (( c=$S; c<=$E; c++ ))
        do
            $CMD >> test_times.txt
        done
    done 
    for T in 2 4 8 16 32 
    do 
        FILE="./omp/main_linear_partial.o"
        CMD="../../"$FILE" ${PARAM} -t ${T}"
        echo $FILE $T >> test_times.txt
        echo $CMD
        for (( c=$S; c<=$E; c++ ))
        do
            $CMD >> test_times.txt
        done
    done 
done
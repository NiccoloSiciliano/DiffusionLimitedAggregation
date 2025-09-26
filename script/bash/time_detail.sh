#!/bin/bash
GRID="1000"
PARTICLES="700000"
ITERATIONS="600"
SEEDX="200"
SEEDY="200"
SEEDZ="200"

PARAM="-g ${GRID} -n ${PARTICLES} -i ${ITERATIONS} -s ${SEEDX} -s ${SEEDY}"
FILE="mainLinear.o"
INFO="----------------------------------------SEQUENTIAL LINEAR VERSION----------------------------
| GRID: ${GRID}  | PARTICLES:  ${PARTICLES}  | ITERATIONS:  ${ITERATIONS}  | SEEDX:  ${SEEDX}  | SEEDY: ${SEEDY} |
---------------------------------------------------------------------------------------------"
CMD="../../"$FILE" ${PARAM} "
echo "$INFO" >  times.txt
echo  "$CMD" >> times.txt
 { time $CMD  ; } 2>> ./times.txt

for T in 2 4 8 16 32 64
do 
    FILE="main_pt_linear.o"
    INFO="----------------------------------------PTHREAD PARALLEL LINEAR VERSION (FAST WITH LESS MUTEX)-----------
| GRID: ${GRID}  | PARTICLES:  ${PARTICLES}  | ITERATIONS:  ${ITERATIONS}  | SEEDX:  ${SEEDX}  | SEEDY: ${SEEDY} | THREADS ${T} |
---------------------------------------------------------------------------------------------"
    CMD="../../"$FILE" ${PARAM} -t $T"
    echo "$INFO" >>  times.txt
    echo $CMD >> times.txt
    { time $CMD ; } 2>> ./times.txt
done 

# FILE="mainomp.o"
# CMD="----------------------------------------OMP PARALLEL VERSION (NO MUTEX)----------------------
# | GRID: ${GRID}  | PARTICLES:  ${PARTICLES}  | ITERATIONS:  ${ITERATIONS}  | SEEDX:  ${SEEDX}  | SEEDY: ${SEEDY} | THREADS ${THREADS} |
# ---------------------------------------------------------------------------------------------"
# echo "$CMD" >>  times.txt
# { time ../../"$FILE" -g ${GRID} -n ${PARTICLES} -i ${ITERATIONS} -s ${SEEDX} -s ${SEEDY} -t ${THREADS} ; } 2>> ./times.txt
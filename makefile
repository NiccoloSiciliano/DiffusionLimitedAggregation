CC := gcc
CFLAGS := -Wall
PROGS := ./sequential/main_linear.o \
	./sequential/main_quadratic.o \
	./pthread/main_quadratic.o \
	./pthread/main_linear_partial.o \
	./pthread/main_linear_fully.o \
	./omp/main_quadratic_parallel.o \
	./omp/main_quadratic_nested.o \
	./omp/main_linear_partial.o \
	./omp/main_linear_fully.o \
	
SEQ := ./util/particleLogic/checkPos.c \
	./util/particleLogic/compareParticle.c \
	./util/graphicUtil/gridToImage.c \
	./util/graphicUtil/printGrid.c \
	./util/graphicUtil/printParticles.c \
	./util/getOption.c 

THR := ./util/threadUtil/barrierUtil.c 
	
.PHONY := all

all : $(PROGS)


./sequential/main_linear.o: ./sequential/main_linear.c $(SEQ)
	$(CC) $(CFLAGS) $^ -lm -o $@

./sequential/main_quadratic.o: ./sequential/main_quadratic.c $(SEQ)
	$(CC) $(CFLAGS) $^ -lm -o $@

./pthread/main_quadratic.o: ./pthread/main_quadratic.c ./pthread/particles_manager_quadratic.c $(THR) $(SEQ) 
	$(CC) $(CFLAGS) $^ -lm -pthread -o $@ 

./pthread/main_linear_partial.o: ./pthread/main_linear_partial.c ./pthread/particles_manager_linear_partial.c $(THR) $(SEQ) 
	$(CC) $(CFLAGS) $^ -lm -pthread -o $@ 

./pthread/main_linear_fully.o: ./pthread/main_linear_fully.c ./pthread/particles_manager_linear_fully.c $(THR) $(SEQ) 
	$(CC) $(CFLAGS) $^ -lm -pthread -o $@ 

./omp/main_quadratic_parallel.o: ./omp/main_quadratic_parallel.c $(SEQ)
	$(CC) $(CFLAGS) $^ -lm -fopenmp -o $@

./omp/main_quadratic_nested.o: ./omp/main_quadratic_nested.c $(SEQ)
	$(CC) $(CFLAGS) $^ -lm -fopenmp -o $@

./omp/main_linear_partial.o: ./omp/main_linear_partial.c $(SEQ)
	$(CC) $(CFLAGS) $^ -lm -fopenmp -o $@ 

./omp/main_linear_fully.o: ./omp/main_linear_fully.c $(SEQ)
	$(CC) $(CFLAGS) $^ -lm -fopenmp -o $@ 

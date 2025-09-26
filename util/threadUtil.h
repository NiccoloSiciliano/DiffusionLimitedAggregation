/*
    Questo file contiene la definizione delle strutture e delle funzioni utili per le implementazioni con la
    libreria pthread.
*/
#ifndef THREAD_UTIL
#define THREAD_UTIL

#include "particleLogic.h"
#include "graphicUtil.h"
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <semaphore.h>

// Barrier
typedef struct {
    int counter;                // Contatore dei thread in attesa sulla Barrier.
    pthread_mutex_t count_mut;  // Mutex per l'accesso al contatore.
    sem_t *barrier_sem[2];      // Array di due semafori utilizzati in modo alternato.
} Barrier;

// TArgs
typedef struct {
    int mynumP;                 // Numero di particelle che deve gestire un singolo thread.
    int firstP;                 // Indice della prima particella affidata al thread.
    int *opt_mask;              // Puntatore alla maschera delle opzioni.
    Particle *actPos;           // Puntatore alla lista delle posizioni attuali delle particelle.
    int *grid;                  // Puntatore alla griglia.
    pthread_mutex_t *mutext;    // Mutex utilizzato per proteggere la fase di spostamento nell'implementazione
                                    // descritta all'interno di ../pthread/particles_manager_linear_partial.c
}TArgs;

extern int dimG, numP, numI, numthread; // Dimensione della griglia, numero di particelle, numero di iterazioni, numero di thread.
extern bool threed;                     // Variabile booleana che indica se le particelle si muovono in una griglia 3D(true) o 2D(false).
extern float axis;                      // Numero di assi della griglia: (x,y,z) o (x,y).
extern Barrier barrierSpawn, barrierCheck, barrierMove; // Barriere usate per sincronizzare i thread. La barriera barrierSpawn è posta 
                                                            // prima della fase di Spawn, barrierCheck è posta tra la fase di controllo 
                                                            // e la fase di spostamento, infine barrierMove è posta tra la fase di spostamento
                                                            // e la fase di controllo.

void particles_manager(void* args);     // Funzione eseguita dai thread implementata in diversi modi all'interno
                                            // dei file ../pthread/particles_manager_*.c

// Funzioni di gestione delle Barrier descritte in /threadUtil/barrierUtil.c
int init_barrier(Barrier *barrier);
int barrier_wait(Barrier *barrier, int counterlimit, int iter);
int destroy_barrier(Barrier *barrier);

#endif
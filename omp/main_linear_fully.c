/*
    Questo file contiene l'implementazione fully parallel con omp dell'algoritmo lineare.
    Per "fully parallel" si intende che tutte le fasi di computazione delle particelle sono eseguite in parallelo.
*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <fcntl.h>
#include <omp.h>

#include "../util/particleLogic.h"
#include "../util/graphicUtil.h"
#include "../util/util.h"

// Variabili globali
Particle nullP = {-1, -1,-1, 0, 0}; // Particella "NULLA" usata come placeholder per indicare l'assenza di dati.
char stringerr[100];                // Stringa contenente un messaggio di errore.

int main(int argc, char *argv[])
{
    int dimG, numP, numI, numT; // Dimensione della griglia, numero di particelle, numero di iterazioni, numero di thread.
    bool threed = false;        // Variabile booleana che indica se le particelle si muovono in una griglia 3D(true) o 2D(false).
    float axis = 2;             // Numero di assi della griglia: (x,y,z) o (x,y).

    int opt_mask[8] = {-1, -1, -1, -1, 0, 1, -1, 0};// Maschera delle opzioni inseribili dall'utente (le opzioni sono definite nel file util/util.h).
    Particle seed;                                  // Cristallo da inserire nella griglia all'inizio della simulazione. 

    // Estrazione delle opzioni dall'input dato dall'utente.
    if( getOption(argc, argv, opt_mask, &seed) < 0)
    {
        printf("%s\n", stringerr); 
        return 0;
    }

    dimG = opt_mask[G];     // Dimensione della griglia.
    numP = opt_mask[N];     // Numero di particelle.
    numT = opt_mask[T];     // Numero di thread.
    numI = opt_mask[I];     // Numero di iterazioni.

    // Allocazione della memoria per la lista delle posizioni attuali delle particelle.
    Particle *actPos = (Particle*)calloc(sizeof(Particle), numP+1);
    // Allocazione della memoria per la lista delle posizioni precedenti delle particelle.
    Particle *prePos = (Particle*)calloc(sizeof(Particle), numP+1);

    // Viene estratto il numero di dimensioni scelto dall'utente.
    if (opt_mask[D]> 0)
    {
        threed = true;
        axis = 3;
    }

    // Allocazione della memoria per la griglia di contatori per segnalare quante particelle sono sulla stessa cella.
    int *grid = calloc(dimG*dimG*((dimG-1)*threed + 1), sizeof(int));
    // Allocazione della memoria per la lista di mutex associati alle celle della griglia.
    omp_lock_t *mutex_grid = calloc(dimG*dimG*((dimG-1)*threed + 1), sizeof(omp_lock_t));

    // Inizializzazione della griglia di mutex.
    for (int i = 0; i < dimG; i++)
        for (int j = 0; j < dimG; j++)
            for (int z = 0; z < (dimG-1)*threed + 1; z++)
                omp_init_lock(mutex_grid + i + dimG*j + dimG*dimG*z);
    
    // Il seme scelto dall'utente viene inserito alla fine della lista di particelle.
    actPos[numP] = seed;
    *(grid + seed.x + dimG*seed.y + dimG*dimG*seed.z) = -1;

    // Variabili per la misurazione del tempo.
    struct timespec start, finish;
    double elapsed;
    // Registrazione del tempo di inizio dell'esecuzione.
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Divisione della griglia in blocchi.
    int rt = (int)ceil(pow(numP,1.f/axis)); // Radice quadrata(2 dimensioni) o radice cubica (3 dimensioni)
    int blockDim = dimG / rt;               // Dimensione di un blocco.   
    int rem = dimG % rt;                    // Resto.

    // Fase di Spawn.
    #pragma omp parallel for num_threads(numT)
    for (int p = 0; p < numP; p++) 
    {          
        // Stabilisce coordinate del blocco associato alla particella p.
        int i = (p/rt)%rt;          // Riga del blocco.
        int j = p%rt;               // Colonna del blocco.
        int h = p/(int)pow(rt, 2);  // Altezza del blocco.

        // Genera la particella in una posizione randomica all'interno del blocco. 
        actPos[p].seed = p + opt_mask[R];
        actPos[p].x = (i)*(i < rem) + blockDim*i + rem*(i >= rem) + rand_r(&(actPos[p].seed))%(blockDim+1*(i<rem));
        actPos[p].y = (j)*(j < rem) + blockDim*j + rem*(j >= rem) + rand_r(&(actPos[p].seed))%(blockDim+1*(j<rem)); 
        actPos[p].z = threed ? ((h)*(h < rem) + blockDim*h + rem*(h >= rem) + rand_r(&(actPos[p].seed))%(blockDim+1*(h<rem))): 0; 
        actPos[p].stuck = 0;    // La particella non è cristallizzata
        int *v = (grid + actPos[p].x + dimG*actPos[p].y + dimG*dimG*actPos[p].z);   // Contatore relativo alla cella occupata da p.
        if (*v >= 0)    // Se la cella è vuota.
        {
            (*v)++;     // Incrementa il contatore.
            actPos[p].stuck = 0;    // La particella non è cristallizzata.
        }
        else    // Se la particella è su una cella occupatta dal cristallo iniziale.
            actPos[p].stuck = 2;    // p è cristallizzata.
        prePos[p] = actPos[p];      // Inizializzazione della lista delle posizioni precedenti delle particelle. 
    }
    
    // Loop delle iterazioni.
    for (int iter = 0; iter < numI; iter++) 
    {
        #pragma omp parallel for num_threads(numT) schedule(guided)
        for (int p1 = 0; p1 < numP; p1++)
        {
            if (actPos[p1].stuck > 0)   // Se una particella è cristallizzata salta l'iterazione.
                continue;
            // Contatore della griglia relativo all'attuale posizione della particella.
            int v = *(grid + actPos[p1].x + dimG*actPos[p1].y + dimG*dimG*actPos[p1].z);
            // Fase per il calcolo della nuova posizione.
            if (v==1)   // Se p1 è l'unica particella all'interno della cella.
            {   
                prePos[p1] = actPos[p1];  // Memorizzazione della posizione attuale per eventuale rimbalzo futuro.
                // Calcolo del movimento randomico.
                actPos[p1].x = actPos[p1].x +  rand_r(&(actPos[p1].seed)) % 3 -1;
                actPos[p1].z = threed ? (actPos[p1].z + rand_r(&(actPos[p1].seed)) % 3 -1): actPos[p1].z;
                
                // Se il movimento sull'asse x e sull'asse z è nullo.
                if (actPos[p1].x == prePos[p1].x && actPos[p1].z == prePos[p1].z)
                    // Il movimento sull'asse y è -1 o +1.
                    actPos[p1].y = actPos[p1].y + (rand_r(&(actPos[p1].seed)) % 2 == 0 ? -1 : 1); 
                else
                    actPos[p1].y = actPos[p1].y + rand_r(&(actPos[p1].seed)) % 3 -1;
                // Controllo sulla validità della nuova posizione (deve essere all'interno della griglia).
                if (!(actPos[p1].x >= 0 && actPos[p1].y >= 0 && actPos[p1].z >= 0) || !(actPos[p1].x < dimG && actPos[p1].y < dimG && actPos[p1].z < dimG) )
                {
                    // Se la posizione non è valida, la particella rimane ferma.
                    int sd = actPos[p1].seed;   // Memorizzazione del seme aggiornato all'ultima iterazione per la funzione random.
                    actPos[p1] = prePos[p1];
                    actPos[p1].seed = sd;
            
                }
            }
            else   // Il contatore è != 1, quindi p1 non è l'unica particella all'interno della cella.
            {
                Particle pos = actPos[p1];
                actPos[p1] = prePos[p1];   // La particella rimbalza nella posizione precedente.
                prePos[p1] = pos;          // Memorizzazione della posizione attuale.
                actPos[p1].seed = pos.seed;// Memorizzazione del seme aggiornato all'ultima iterazione per la funzione random.
                if (v < 0)  // Se p1 si trova nella stessa cella di un cristallo.
                    actPos[p1].stuck = 1;   // p1 inizia la cristallizzazione.
            }      
        }
    
        // Fase di spostamento.
        #pragma omp parallel for num_threads(numT)
        for (int p = 0; p < numP; p++)
        {
            switch (actPos[p].stuck)
            {
                case 0:     // Se la particella non è cristallizzata.
                {
                    // Lock sul contatore relativo alla cella della griglia nella quale si trova la particella. 
                    omp_set_lock(mutex_grid + actPos[p].x + dimG*actPos[p].y + dimG*dimG*actPos[p].z); 
                    int *v = grid + actPos[p].x + dimG*actPos[p].y + dimG*dimG*actPos[p].z; // Contatore della cella.
                    if (*v >= 0)    // Se non c'è un cristallo sulla cella.
                        (*v)++;     // Incrementa il contatore.
                    // Unlock del contatore.
                    omp_unset_lock(mutex_grid + actPos[p].x + dimG*actPos[p].y + dimG*dimG*actPos[p].z); 
                    
                    // Lock sul contatore relativo alla cella della griglia nella quale si trovava la particella.
                    omp_set_lock(mutex_grid + prePos[p].x + dimG*prePos[p].y + dimG*dimG*prePos[p].z);
                    int *prev = grid + prePos[p].x + dimG*prePos[p].y + dimG*dimG*prePos[p].z;
                    (*prev)--;  // Decrementa il contatore.
                    // Unlock del contatore.
                    omp_unset_lock(mutex_grid + prePos[p].x + dimG*prePos[p].y + dimG*dimG*prePos[p].z);  
                    break;
                }
                case 1: // Se la particella si sta cristallizzando.
                {
                    // Lock sul contatore relativo alla cella della griglia nella quale si trova la particella.
                    omp_set_lock(mutex_grid + actPos[p].x + dimG*actPos[p].y + dimG*dimG*actPos[p].z); 
                    int *v = grid + actPos[p].x + dimG*actPos[p].y + dimG*dimG*actPos[p].z;
                    if (*v >= 0)    // Se non c'è già un cristallo.
                        *v = -1*(*v+1); // Il contatore diventa negativo per indicare la presenza di un cristallo.
                    // Unlock del contatore.
                    omp_unset_lock(mutex_grid + actPos[p].x + dimG*actPos[p].y + dimG*dimG*actPos[p].z); 
                    actPos[p].stuck = 2;    // La particella è cristallizzata.
                    break;
                }
            }
        }
    }

    // Registrazione del tempo di completamento dell'esecuzione.
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("%f\n", elapsed);

    // Se specificato dall'utente viene generata un'immagine .png del cristallo finale.
    if (opt_mask[P] >= 0)
        gridToImage(actPos, numP+1, dimG);
    
    // Se specificato dall'utente viene generata la lista delle posizioni finali delle particelle.
    if (opt_mask[C] >= 0)
        printParticles(actPos, numP+1, dimG);

    free(actPos);
    free(prePos);
    free(grid);
    free(mutex_grid);
        
    return 0;
}
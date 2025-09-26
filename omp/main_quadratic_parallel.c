/*
    Questo file contiene l'implementazione parallela dell'algoritmo quadratico
    parallelizzata con la libreria omp.
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
    int dimG, numP, numI, numT=1;   // Dimensione della griglia, numero di particelle, numero di iterazioni.
    bool threed = false;            // Variabile booleana che indica se le particelle si muovono in una griglia 3D(true) o 2D(false).
    float axis = 2;                 // Numero di assi della griglia: (x,y,z) o (x,y).
    
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
    numI = opt_mask[I];     // Numero di iterazioni.
    numT = opt_mask[T];     // Numero di thread.   
 
    // Viene estratto il numero di dimensioni scelto dall'utente.
    if (opt_mask[D]> 0)
    {
        threed = true;
        axis = 3;
    }
    
    // Allocazione della memoria per la lista delle posizioni attuali delle particelle.
    Particle *actPos = (Particle*)calloc(sizeof(Particle),numP+1);  

    // Il seme scelto dall'utente viene inserito alla fine della lista di particelle.
    actPos[numP] = seed;

    // Allocazione della memoria per la lista delle posizioni successive delle particelle.
    Particle* postPos = (Particle*)calloc(sizeof(Particle),numP);
    // Allocazione della memoria per la lista delle posizioni precedenti delle particelle.
    Particle* prePos = (Particle*)calloc(sizeof(Particle),numP);

    // Variabili per la misurazione del tempo.
    struct timespec start, finish;
    double elapsed;
    // Registrazione del tempo di inizio dell'esecuzione.
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Divisione della griglia in blocchi.
    int rt = (int)ceil(pow(numP,1.f/axis)); // Radice quadrata(2 dimensioni) o radice cubica (3 dimensioni).
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
        actPos[p].stuck = false;    // La particella non è cristallizzata.

        prePos[p] = actPos[p];   // Inizializzazione della lista delle posizioni precedenti delle particelle.

        postPos[p] = nullP;      // Inizializzazione della lista delle posizioni successive delle particelle.
        postPos[p].seed = actPos[p].seed;    // Memorizzazione del seed per la funzione random.
    }

    // Loop delle iterazioni.
    for (int iter = 0; iter < numI; iter++) 
    {
        #pragma omp parallel for num_threads(numT) schedule(guided)
        for (int p1 = 0; p1 < numP; p1++)   // Ogni particella viene confrontata.
        {
            if (actPos[p1].stuck)   // Se una particella è cristallizzata salta l'iterazione.
                continue;
                
            // Fase di controllo.
            for (int p2 = 0; p2 < numP+1; p2++) // Con tutte le altre particelle.
            {
                // Verifica del rimbalzo di due particelle (dichiarata in util/particleLogic/checkPos.c)
                Particle newPost = checkPos(p1, p2, actPos, prePos[p1]);
            
                // Se avviene un rimbalzo.
                if (!compareParticle(newPost, nullP))
                {
                    if (iter == 0)
                        postPos[p1] = actPos[p1]; // La particella rimane ferma.
                    else
                    {
                        /* Se la particella si trova sulla stessa cella con un cristallo
                            e altre particelle l'ordine con cui vengono calcolati i rimbalzi
                            potrebbe influire sulla cristallizzazione della particella. 
                        */
                        if (postPos[p1].stuck)  // Se si è verificato un rimbalzo con un cristallo.
                            newPost.stuck = 2;  // La particella è cristallizzata.
                        postPos[p1] = newPost;  // La nuova posizione è quella calcolata nel rimbalzo.
                    }
                }
            }
                
            // Fase per il calcolo della nuova posizione. 
            if (compareParticle(postPos[p1], nullP))  // Se non c'è stato un rimbalzo.
            {
                // Calcolo del movimento randomico.
                int r = rand_r(&(actPos[p1].seed));
                postPos[p1].x = actPos[p1].x + r % 3 -1;
                postPos[p1].z = threed ? (actPos[p1].z + rand_r(&(actPos[p1].seed)) % 3 -1): actPos[p1].z;

                // Se il movimento sull'asse x e sull'asse z è nullo.
                if (postPos[p1].x == actPos[p1].x && postPos[p1].z == actPos[p1].z)
                    // Il movimento sull'asse y è -1 o +1.
                    postPos[p1].y = actPos[p1].y + (rand_r(&(actPos[p1].seed)) % 2 == 0 ? -1 : 1); 
                else
                    postPos[p1].y = actPos[p1].y + rand_r(&(actPos[p1].seed)) % 3 -1;

                // Controllo sulla validità della nuova posizione (deve essere all'interno della griglia).
                if (!(postPos[p1].x >= 0 && postPos[p1].y >= 0 && postPos[p1].z >= 0) || !(postPos[p1].x < dimG && postPos[p1].y < dimG && postPos[p1].z < dimG) )
                    // Se la posizione non è valida, la particella rimane ferma.
                    postPos[p1] = actPos[p1];

                postPos[p1].seed = actPos[p1].seed;   // Memorizzazione del seme aggiornato all'ultima iterazione per la funzione random.
            }          
        }
        
        // Fase di spostamento.
        #pragma omp parallel for num_threads(numT) schedule(guided)
        for (int p = 0; p < numP; p++)
        {
            if (!actPos[p].stuck)   // Se la particella non è cristallizzata.
            {
                // Aggiorna le liste delle posizioni.
                prePos[p] = actPos[p];
                actPos[p] = postPos[p];
                postPos[p] = nullP;
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
    free(postPos);
    free(prePos);
    
    return 0;
}
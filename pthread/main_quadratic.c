/*
    Questo file contiene l'implementazione parallela con Pthread dell'algoritmo quadratico.
*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>

#include "../util/particleLogic.h"
#include "../util/graphicUtil.h"
#include "../util/util.h"
#include "../util/threadUtil.h"

// Variabili globali
int dimG, numP, numI, numthread=1;              // Dimensione della griglia, numero di particelle, numero di iterazioni, numero di thread.
bool threed = false;                            // Variabile booleana che indica se le particelle si muovono in una griglia 3D(true) o 2D(false).     
float axis = 2;                                 // Numero di assi della griglia: (x,y,z) o (x,y).
Barrier barrierSpawn, barrierCheck, barrierMove;// Barriere usate per sincronizzare i thread. La barriera barrierSpawn è posta 
                                                    // prima della fase di Spawn, barrierCheck è posta tra la fase di controllo 
                                                    // e la fase di spostamento, infine barrierMove è posta tra la fase di spostamento e la fase di
                                                    // controllo.
Particle nullP = {-1, -1,-1, 0, 0};             // Particella "NULLA" usata come placeholder per indicare l'assenza di dati.
char stringerr[100];                            // Stringa contenente un messaggio di errore.

int main(int argc, char *argv[])
{

    int opt_mask[8] = {-1, -1, -1, -1, 0, 1, -1, 0};// Maschera delle opzioni inseribili dall'utente (le opzioni sono definite nel file util/util.h).
    Particle seed;                                  // Cristallo da inserire nella griglia all'inizio della simulazione.

    // Estrazione delle opzioni dall'input dato dall'utente.
    if( getOption(argc, argv, opt_mask, &seed) < 0)
    {
        printf("%s\n", stringerr); 
        return 0;
    }

    dimG = opt_mask[G];                             // Dimensione della griglia.
    numP = opt_mask[N];                             // Numero di particelle.
    numI = opt_mask[I];                             // Numero di iterazioni.
    numthread = opt_mask[T];                        // Numero di thread.

    TArgs targs[numthread];                         // Vettore di strutture contenenti gli argomenti per ogni thread (TArgs definita in util/threadUtil.h).
    pthread_t thread_id[numthread];                 // Vettore di indentificativi per i thread.
    Particle actPos[numP+1];                        // Vettore per la lista di particelle (Particel definita in util/particleLogic.h).
 
    // Inizializzazione delle barriere (init_barrier definita in util/threadUtil/barrierUtil.c).
    init_barrier(&barrierSpawn);
    init_barrier(&barrierCheck);
    init_barrier(&barrierMove);

    // Viene estratto il numero di dimensioni scelto dall'utente.
    if (opt_mask[D]> 0)
    {
        threed = true;
        axis = 3;
    }
    
    // Il seme scelto dall'utente viene inserito alla fine della lista di particelle.
    actPos[numP] = seed;

    // Creazione dei thread e inizializzazione degli argomenti.
    for (int t = 0; t < numthread; t++)
    {
        // Suddivisione delle particelle tra i vari thread.
        targs[t].mynumP = numP/numthread + (t < numP%numthread ? 1 : 0);
        targs[t].firstP = (numP/numthread) * t + (t < numP%numthread ? t : numP%numthread);
        
        targs[t].actPos = actPos; 
        targs[t].opt_mask = opt_mask;
        // Avvio dei thread che eseguono la funzione particles_manager (dichiarata in pthread/particle_manager_quadratic.c)
        pthread_create(thread_id+t, NULL, (void *)particles_manager, (void*)(targs + t));
    }
    
    // Il thread master si mette in attesa degli altri thread.
    for (int t = 0; t < numthread; t++)
        pthread_join(*(thread_id+t), NULL);
    
    // Se specificato dall'utente viene generata un'immagine .png del cristallo finale.
    if (opt_mask[P] >= 0)
        gridToImage(actPos, numP+1, dimG);
    
    // Se specificato dall'utente viene generata la lista delle posizioni finali delle particelle.
    if (opt_mask[C] >= 0)
        printParticles(actPos, numP+1, dimG);

    // Distruzione delle barriere.
    destroy_barrier(&barrierSpawn);
    destroy_barrier(&barrierMove);
    destroy_barrier(&barrierCheck);

    return 0;
}
/*
    Questo file contiene la funzione eseguita da ogni thread del programma "partially parallel"(pthread/main_linear_partial.c).
*/
#include "../util/threadUtil.h"
#include "../util/util.h"

void particles_manager(void* args)
{
    TArgs* myargs = (TArgs*) args;
        
    // Estrazione degli argomenti.
    int mynumP = myargs->mynumP;
    int firstP = myargs->firstP;
    int lastP = firstP + mynumP;
    int *grid = myargs->grid;
    pthread_mutex_t *mutext = myargs->mutext;

    Particle *actPos = myargs->actPos;
    // Allocazione della memoria per la lista delle posizioni precedenti delle particelle.
    Particle* prePos = (Particle*)calloc(sizeof(Particle),mynumP);

    // Variabili per la misurazione del tempo.
    struct timespec start, finish;
    double elapsed;
    // Registrazione del tempo di inizio dell'esecuzione eseguita dal thread 0.
    barrier_wait(&barrierSpawn, numthread, 0);
    if (firstP == 0)
        clock_gettime(CLOCK_MONOTONIC, &start);

    // Divisione della griglia in blocchi.
    int rt = (int)ceil(pow(numP,1.f/axis)); // Radice quadrata(2 dimensioni) o radice cubica (3 dimensioni).
    int blockDim = dimG / rt;               // Dimensione di un blocco.
    int rem = dimG % rt;                    // Resto.

    // Fase di Spawn.
    for (int p = firstP; p < lastP; p++) 
    {          
        // Stabilisce coordinate del blocco associato alla particella p.
        int i = (p/rt)%rt;          // Riga del blocco.
        int j = p%rt;               // Colonna del blocco.
        int h = p/(int)pow(rt, 2);  // Altezza del blocco.
        int indp = -1*(firstP - p); // Mappa l'indice p nell'intervallo [0, mynumP] per poter accedere al vettore prePos.
        
        // Genera la particella in una posizione randomica all'interno del blocco.
        actPos[p].seed = p + myargs->opt_mask[R];
        actPos[p].x = (i)*(i < rem) + blockDim*i + rem*(i >= rem) + rand_r(&(actPos[p].seed))%(blockDim+1*(i<rem));
        actPos[p].y = (j)*(j < rem) + blockDim*j + rem*(j >= rem) + rand_r(&(actPos[p].seed))%(blockDim+1*(j<rem)); 
        actPos[p].z = threed ? ((h)*(h < rem) + blockDim*h + rem*(h >= rem) + rand_r(&(actPos[p].seed))%(blockDim+1*(h<rem))): 0; 
        
        // Aggiornamento della griglia.
        // In questo caso non sono necessari lock perchè all'inizio le particelle sono tutte in posizioni diverse.
        int *v = (grid + actPos[p].x + dimG*actPos[p].y + dimG*dimG*actPos[p].z);
        // Controllo del caso in cui una particella nasca sopra il seme iniziale.
        if (*v >= 0)
        {
            (*v)++;
            actPos[p].stuck = 0;    // Particella.   
        }
        else
            actPos[p].stuck = 2;    // Cristallo.
        
        prePos[indp] = actPos[p];   // Memorizzazione della posizione attuale per eventuale rimbalzo futuro.
    }

    // Non è necessario attendere la fase di spawn degli altri thread pechè all'inizio 
    // le particelle sono tutte in posizioni diverse.

    // Loop delle iterazioni. 
    for (int iter = 0; iter < numI; iter++) 
    {
        int indp;   // Indice nell'intervallo [0, mynumP].
        for (int p1 = firstP; p1 < lastP; p1++)
        {
            indp = -1*(firstP - p1);    // Calcolo dell'indice parziale.
            if (actPos[p1].stuck > 0)   // Se una particella è cristallizzata salta l'iterazione.
                continue;

            // Contatore della griglia relativo all'attuale posizione della particella.
            int v = *(grid + actPos[p1].x + dimG*actPos[p1].y + dimG*dimG*actPos[p1].z);

            // Fase per il calcolo della nuova posizione.
            if (v==1)   // Se p1 è l'unica particella all'interno della cella.
            {   
                prePos[indp] = actPos[p1];  // Memorizzazione della posizione attuale per eventuale rimbalzo futuro.
                // Calcolo del movimento randomico.
                actPos[p1].x = actPos[p1].x +  rand_r(&(actPos[p1].seed)) % 3 -1;
                actPos[p1].z = threed ? (actPos[p1].z + rand_r(&(actPos[p1].seed)) % 3 -1): actPos[p1].z;

                // Se il movimento sull'asse x e sull'asse z è nullo.
                if (actPos[p1].x == prePos[indp].x && actPos[p1].z == prePos[indp].z)
                    // Il movimento sull'asse y è -1 o +1.
                    actPos[p1].y = actPos[p1].y + (rand_r(&(actPos[p1].seed)) % 2 == 0 ? -1 : 1); 
                else
                    actPos[p1].y = actPos[p1].y + rand_r(&(actPos[p1].seed)) % 3 -1;
                // Controllo sulla validità della nuova posizione (deve essere all'interno della griglia).
                if (!(actPos[p1].x >= 0 && actPos[p1].y >= 0 && actPos[p1].z >= 0) || !(actPos[p1].x < dimG && actPos[p1].y < dimG && actPos[p1].z < dimG) )
                    {
                        // Se la posizione non è valida, la particella rimane ferma.
                        int sd = actPos[p1].seed;   // Memorizzazione del seme aggiornato all'ultima iterazione per la funzione random.
                        actPos[p1] = prePos[indp];
                        actPos[p1].seed = sd;
                    }
            }
            else    // Il contatore è != 1, quindi p1 non è l'unica particella all'interno della cella.
            {
                Particle pos = actPos[p1];
                actPos[p1] = prePos[indp];  // La particella rimbalza nella posizione precedente.
                prePos[indp] = pos;         // Memorizzazione della posizione attuale.
                actPos[p1].seed = pos.seed; // Memorizzazione del seme aggiornato all'ultima iterazione per la funzione random.
                if (v < 0)  // Se p1 si trova nella stessa cella di un cristallo.
                    actPos[p1].stuck = 1; // p1 inizia la cristallizzazione.
            }
        }

        // Barriera per aspettare il completamento della fase di calcolo della nuova posizione, da parte di tutti i thread.
        barrier_wait(&barrierCheck, numthread, iter);
        // Fase di spostamento.
        // L'intera fase è eseguita in modo sequenziale
        pthread_mutex_lock(mutext); 
        for (int p = firstP; p < lastP; p++)
        {
            indp = -1*(firstP - p); // Calcolo dell'indice parziale.
            switch (actPos[p].stuck)
            {
                case 0: // Se la particella non è cristallizzata.
                {
                    // Contatore relativo alla cella della griglia nella quale si trova la particella. 
                    int *v = grid + actPos[p].x + dimG*actPos[p].y + dimG*dimG*actPos[p].z;
                    
                    if (*v >= 0)    // Se non c'è un cristallo sulla cella.
                        (*v)++;     // Incrementa il contatore. 
                    
                    // Contatore relativo alla cella della griglia nella quale si trovava la particella.
                    int *prev = grid + prePos[indp].x + dimG*prePos[indp].y + dimG*dimG*prePos[indp].z;
                    (*prev)--;  // Decrementa il contatore.
                    break;
                }
                case 1: // Se la particella si sta cristallizzando.
                {
                    // Contatore relativo alla cella della griglia nella quale si trova la particella. 
                    int *v = grid + actPos[p].x + dimG*actPos[p].y + dimG*dimG*actPos[p].z;
                    if (*v >= 0)    // Se non c'è già un cristallo.
                        *v = -1*(*v+1); // Il contatore diventa negativo per indicare la presenza di un cristallo.
                    actPos[p].stuck = 2;    // La particella è cristallizzata.
                    break;
                }
            }
        }
        // Fine della fase sequenziale.
        pthread_mutex_unlock(mutext); 
        
        // Barriera per aspettare il completamento della fase di spostamento, da parte di tutti i thread.
        barrier_wait(&barrierMove, numthread, iter);
            
    }

    // Registrazione del tempo di completamento dell'esecuzione eseguita dal thread 0.
    barrier_wait(&barrierSpawn, numthread, 0);
    if (firstP == 0)
    {
        clock_gettime(CLOCK_MONOTONIC, &finish);

        elapsed = (finish.tv_sec - start.tv_sec);
        elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
        printf("%f\n", elapsed);
    }
    
    free(prePos);
}
/*
    Questo file contiene la funzione eseguita da ogni thread del programma (pthread/main_quadratic.c).
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

    Particle *actPos = myargs->actPos;
    // Allocazione della memoria per la lista delle posizioni successive delle particelle.
    Particle* postPos = (Particle*)calloc(sizeof(Particle),mynumP);
    // Allocazione della memoria per la lista delle posizioni precedenti delle particelle.
    Particle* prePos = (Particle*)calloc(sizeof(Particle),mynumP);

    // Divisione della griglia in blocchi.
    int rt = (int)ceil(pow(numP,1.f/axis)); // Radice quadrata(2 dimensioni) o radice cubica (3 dimensioni)
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
        actPos[p].stuck = false;    // La particella non è cristallizzata.

        prePos[indp] = actPos[p];   // Inizializzazione della lista delle posizioni precedenti delle particelle.

        postPos[indp] = nullP;      // Inizializzazione della lista delle posizioni successive delle particelle.
        postPos[indp].seed = actPos[p].seed;    // Memorizzazione del seed per la fuzione random.
    }

    // Barriera per aspettare il completamento della fase di spawn da parte di tutti i thread.
    barrier_wait(&barrierSpawn, numthread, 0);

    // Loop delle iterazioni.
    for (int iter = 0; iter < numI; iter++) 
    {
        int indp;   // Indice nell'intervallo [0, mynumP].
        
        for (int p1 = firstP; p1 < lastP; p1++)
        {
            indp = -1*(firstP - p1);
            if (actPos[p1].stuck)   // Se una particella è cristallizzata salta l'iterazione.
                continue;
                
            // Fase di controllo.
            for (int p2 = 0; p2 < numP+1; p2++)
            {
                // Verifica del rimbalzo di due particelle (dichiarata in util/particleLogic/checkPos.c)
                Particle newPost = checkPos(p1, p2, actPos, prePos[indp]);

                // Se avviene un rimbalzo.
                if (!compareParticle(newPost, nullP))
                {
                    if (iter == 0)
                        postPos[indp] = actPos[p1]; // La particella rimane ferma.
                    else
                    {   
                        /* Se la particella si trova sulla stessa cella con un cristallo
                            e altre particelle l'ordine con cui vengono calcolati i rimbalzi
                            potrebbe influire sulla cristallizzazione della particella. 
                        */
                        if (postPos[indp].stuck)    // Se si è verificato un rimbalzo con un cristallo.
                            newPost.stuck = 2;      // La particella è cristallizzata.
                        postPos[indp] = newPost;    // La nuova posizione è quella calcolata nel rimbalzo.
                    }
                }
            }
                
            // Fase per il calcolo della nuova posizione. 
            if (compareParticle(postPos[indp], nullP))  // Se non c'è stato un rimbalzo.
            {
                // Calcolo del movimento randomico.
                int r = rand_r(&(actPos[p1].seed));
                postPos[indp].x = actPos[p1].x + r % 3 -1;
                postPos[indp].z = threed ? (actPos[p1].z + rand_r(&(actPos[p1].seed)) % 3 -1): actPos[p1].z;

                // Se il movimento sull'asse x e sull'asse z è nullo.
                if (postPos[indp].x == actPos[p1].x && postPos[indp].z == actPos[p1].z)
                    // Il movimento sull'asse y è -1 o +1.
                    postPos[indp].y = actPos[p1].y + (rand_r(&(actPos[p1].seed)) % 2 == 0 ? -1 : 1); 
                else
                    postPos[indp].y = actPos[p1].y + rand_r(&(actPos[p1].seed)) % 3 -1;
                // Controllo sulla validità della nuova posizione (deve essere all'interno della griglia).
                if (!(postPos[indp].x >= 0 && postPos[indp].y >= 0 && postPos[indp].z >= 0) || !(postPos[indp].x < dimG && postPos[indp].y < dimG && postPos[indp].z < dimG) )
                    // Se la posizione non è valida, la particella rimane ferma.
                    postPos[indp] = actPos[p1];   

                postPos[indp].seed = actPos[p1].seed;   // Memorizzazione del seme aggiornato all'ultima iterazione per la funzione random.
            }
            
        }

        // Barriera per aspettare il completamento della fase di calcolo della nuova posizione, da parte di tutti i thread.
        barrier_wait(&barrierCheck, numthread, iter);
        
        // Fase di spostamento.
        for (int p = firstP; p < lastP; p++)
        {
            indp = -1*(firstP - p); // Calcolo dell'indice parziale.
            if (!actPos[p].stuck)   // Se la particella non è cristallizzata.
            {   
                // Aggiorna le liste delle posizioni.
                prePos[indp] = actPos[p];   
                actPos[p] = postPos[indp];
                postPos[indp] = nullP;
            }
        }
        
        // Barriera per aspettare il completamento della fase di spostamento, da parte di tutti i thread.
        barrier_wait(&barrierMove, numthread, iter);
    }

    free(postPos);
    free(prePos);
}
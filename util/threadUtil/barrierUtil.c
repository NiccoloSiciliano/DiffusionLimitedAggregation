/*
    Questo file contiene le funzioni di inizializzazione, distruzione e attesa per le barriere.
    La struttura Barrier è definita nel file threadUtil.h
*/
#include "../threadUtil.h"

int init_barrier(Barrier *barrier)
{
    static int ind = 1;
    int e;
    barrier->counter = 0;   // Inizializzazione couter a 0.
    e = pthread_mutex_init(&(barrier->count_mut), NULL);    // Inizializzazione mutex per il counter
    if (e < 0)
        goto ERROR;
    char name[3];   // Nome per i named semaphore.
    sprintf(name, "%d", ind);   // Inizializzazione del nome.
    barrier->barrier_sem[0] = sem_open(name, O_CREAT, 0644, 0); // Creazione del primo named semaphore.
    if (barrier->barrier_sem[0] == SEM_FAILED)
        goto ERROR;
    ind++;
    sprintf(name, "%d", ind);   // Creazione nome per il secondo semaforo.
    barrier->barrier_sem[1] = sem_open(name, O_CREAT, 0644, 0); // Creazione del secondo named semaphore.
    if (barrier->barrier_sem[1] == SEM_FAILED)
        goto ERROR;
    ind++;
    return 0;

    ERROR:
        return -1;
}


int barrier_wait(Barrier *barrier, int counterlimit, int iter)
{
    int e;
    e = pthread_mutex_lock(&(barrier->count_mut));  // Lock sul contatore della barriera.
    if (e < 0)
        goto ERROR;
    
    if (barrier->counter == counterlimit-1) // Se il thread chiamanate è l'ultimo ad arrivare alla barriera.
    {
        barrier->counter = 0;   // Azzera il contatore
        e = pthread_mutex_unlock(&(barrier->count_mut));    // Unlock sul contatore.
        if (e < 0)
            goto ERROR;
        for (int j =0; j<counterlimit-1; j++)
        {
            e = sem_post(barrier->barrier_sem[iter%2]);     // Risveglia tutti gli altri thread
            if (e < 0)
                goto ERROR;
        }
    }
    else    // Se il thread chiamanate non è l'ultimo ad arrivare alla barriera.
    {
        barrier->counter++; // Incrementa il contatore.
        e = pthread_mutex_unlock(&(barrier->count_mut));    // Unlock sul contatore.
        if (e < 0)
            goto ERROR;
        e = sem_wait(barrier->barrier_sem[iter%2]); // Si mette in attesa sul semaforo.
        if (e < 0)
            goto ERROR;
    }
    return 0;
    ERROR:
        return -1;
}

int destroy_barrier(Barrier *barrier)
{
    int e;
    e = pthread_mutex_destroy(&(barrier->count_mut));   // Distrugge il mutex.
    if (e < 0)
        goto ERROR;
    e = sem_close(barrier->barrier_sem[0]); // Distrugge il primo semaforo.
    if (e < 0)
        goto ERROR;
    e = sem_close(barrier->barrier_sem[1]); // Distrugge il secondo semaforo.
    if (e < 0)
        goto ERROR;
    return 0;
    ERROR: 
        return -1;
}
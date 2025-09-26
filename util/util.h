/*
    Questo file contiene le costanti che riguardano le opzioni del'applicazione.
*/
#ifndef UTIL
#define UTIL

#define P 0     // Opzione per creare l'immagine del cristallo finale.
#define D 1     // Numero di dimensioni della griglia.
#define G 2     // Dimensione del lato della griglia.
#define C 3     // Indica di stampare le posizioni finali delle particelle.
#define I 4     // Numero di iterazioni.
#define T 5     // Numero di thread.
#define N 6     // Numero di particelle.
#define R 7     // Seed per inizializzare la funzione random per il movimento delle particelle.
#define OPTION "-pg:ci:s:t:n:r:"    // Costante da utilizzare per la funzione getopt.

#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include "./particleLogic.h"

extern char stringerr[100]; // Stringa contenente un messaggio di errore.

int getOption(int argc, char *argv[], int *opt_mask, Particle* seed);   // Funzione dichairata in getOption.c
#endif 
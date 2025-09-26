/*
    Questo file contiene l'implementazione della funzione getOption che serve a prendere le opzioni
    inserite dall'utente nella riga di comando.
    
    INPUT: 
        argc        // Numero di parametri inseriti dall'utente.
        *argv[]     // Parametri inseriti dall'utente.
        *opt_mask   // Puntatore alla maschera delle opzioni.
        *seed       // Puntatore al cristallo iniziale.
    OUTPUT:
        restituisce un numero negativo se i parametri sono in una forma sbagliata.

    Le costanti utilizzate in questa funzione sono definite nel file util.h .
*/
#include "./util.h"

int getOption(int argc, char *argv[], int *opt_mask, Particle* seed)
{
    char c;
    int seed_coord[3] = {0};    // Lista delle coordinate del cristallo iniziale.
    int seed_coord_count = 0;   // Contatore per le coordinate del seme (2, versione 2D; 3, versione 3D)
    while((c = getopt(argc, argv, OPTION)) > 0)
    {
            switch(c)
            {
                case 'p':       // L'utente richiede di creare l'immagine del cristallo.
                    *(opt_mask+P) = 1;
                break;
                case 'g':       // Dimensione del lato della griglia.
                    *(opt_mask+G) = atoi(optarg);
                break; 
                case 'c':       // L'utente richiede di scrivere le posizioni finali delle particelle.
                    *(opt_mask+C) = 1;
                break; 
                case 'i':       // Numero di iterazioni.
                    *(opt_mask+I) = atoi(optarg);
                break; 
                case 's':       // Coordinata del seme.
                    seed_coord[seed_coord_count++] = atoi(optarg);
                break; 
                case 'n':       // Numero di particelle da inserire nella griglia.
                    *(opt_mask+N) = atoi(optarg);
                break;
                case 't':       // Numero di thread.
                    *(opt_mask+T) = atoi(optarg);
                break; 
                case 'r':       // Seed per inizializzare la funzione random per il movimento delle particelle.
                    *(opt_mask+R) = atoi(optarg);
                break; 
                case '?':       // Carattere restituito da getopt in caso di errore.
                    return -1;
                break;
            }
    }

    // Inizializzazione delle coordinate del cristallo iniziale.
    seed->x = seed_coord[0];
    seed->y = seed_coord[1];
    seed->z = seed_coord[2];
    seed->stuck = 2;

    if (seed_coord_count >= 3)
        *(opt_mask+D) = 1;      // Indica che la griglia deve avere 3 dimensioni.

    int g =  *(opt_mask+G), n = *(opt_mask+N);

    // Error checking

    if (g <= 0 || n <= 0)   // Se i parametri per la griglia o per il numero di particelli mancano.
    {
        strcpy(stringerr, "Grid dimension or particles number missing.");
        return -1;
    }

    if (seed_coord_count <= 1 || seed_coord_count > 3)  // Se il numero di coordinate per il cristallo iniziale è sbagliato.
    {
        strcpy(stringerr, "Seed coordinates missing.");
        return -1;
    }

    // Se le coordinate del cristallo iniziale sono fuori dalla griglia.
    if ((seed->x < 0 || seed->y < 0 || seed->z < 0) || (seed->x >=  g || seed->y >= g || seed->z >= g))
    {
        strcpy(stringerr, "Seed coordinates out of grid.");
        return -1;
    }
    
    // Se il numero di particelle è superiore a quelle che può contenere la griglia.
    if (n > pow(g, seed_coord_count))
    {
        strcpy(stringerr, "Too many particles.");
        return -1;
    }

    return 0;
}
/*
    Questo file contiene la funzione che scrive le posizioni finali delle particelle
    all'interno della griglia nel file crystal.txt (opzione -c). Utilizzata anche per tracciare
    il grafico della versione 3D.

    INPUT:
        particles[]     // Array delle posizioni delle particelle.
        numP            // Numero di particelle.
        dim             // Dimensione della griglia.
*/
#include "../graphicUtil.h"

void printParticles(Particle particles[], int numP, int dim)
{
    FILE* crystaltxt = fopen("crystal.txt", "w+");  // Apertura o creazione del file crystal.txt
    fprintf(crystaltxt,"%d\n", dim);                // Inserimento della dimensione della griglia nel file.
    for (int p = 0; p < numP; p++)
    {
        if (particles[p].stuck) // Se una particella è cristallizzata, viene seganalato con il carattere 'c'.
            fprintf(crystaltxt,"%d %d %d %c\n", particles[p].x,particles[p].y,particles[p].z, 'c');
        else    // Se una particella non è cristallizzata, viene seganalato con il carattere 'p'.
            fprintf(crystaltxt,"%d %d %d %c\n", particles[p].x,particles[p].y,particles[p].z, 'p');
    } 
}
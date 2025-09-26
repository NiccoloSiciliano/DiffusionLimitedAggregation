/*
    Questo file contiene la funzione per stampare la griglia delle particelle.

    INPUT:
        dim     // Dimensione della griglia.
        *grid   // Lista delle posizioni delle particelle.
*/
#include "../graphicUtil.h"

void printGrid(int dim, char* grid)
{
    for (int i = 0; i < dim; i++) 
    {
        for (int j = 0; j < dim; j++) 
            printf("%c ", *(grid + i*dim + j));     // Le costanti dei caratteri sono definite nel file graphicUtil.h
        printf("\n");
    }        
    printf("\n\n");
}
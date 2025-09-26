/*
    Questo file contiene la funzione di controllo per il rimbalzo di due particelle.
    INPUT:
        p1, p2  // Le particelle da controllare.
        actPos  // Puntatore alla lista delle posizioni attuali delle particelle.
        prePos  // Posizione precedente di p1.
    OUTPUT:
        newp    // La nuova posizione di p1.
*/
#include "../particleLogic.h"

Particle checkPos(int p1, int p2, Particle actPos[],Particle prePos){
    Particle newp = nullP;  // Inizializzazione di newp.

    // Se le p1 e p2 sono sulla stessa cella.
    if (p1 != p2 && actPos[p1].x == actPos[p2].x && actPos[p1].y == actPos[p2].y && actPos[p1].z == actPos[p2].z) 
    {
        newp = prePos;  // La nuova posizione è quella precedente.
        if (actPos[p2].stuck)   // Se p2 è cristallizzata.
            newp.stuck = 1; // p1 si cristallizza.
    }

    return newp;
}


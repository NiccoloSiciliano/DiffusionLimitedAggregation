/*
    Questo file contiene la funzione per la comparazione delle posizioni di due particelle.
    INPUT:
        p1, p2  // Le particelle da controllare.
    OUTPUT:
        true se le posizioni sono uguali
*/
#include "../particleLogic.h"


bool compareParticle(Particle p1, Particle p2)
{
    return p1.x == p2.x && p1.y == p2.y && p1.z == p2.z;
}
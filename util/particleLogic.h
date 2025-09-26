/*
    Questo file contiene la definizione della struttura e delle funzioni utili per gestire le particelle.
*/
#ifndef PARTICLELOGIC_H
#define PARTICLELOGIC_H
#include <stdio.h>
#include <stdbool.h>

// Particle
typedef struct{
    int x;              // Coordinata x.
    int y;              // Coordinata y.
    int z;              // Coordinata z.
    unsigned int seed;  // Seed per la funzione random, unico per ogni particella.
    int stuck;          // 0: particella; 1: inizio cristallizzazione; 2: cristallizzata
}Particle;

extern Particle nullP;  // Particella "NULLA" usata come placeholder per indicare l'assenza di dati.

bool compareParticle(Particle p1, Particle p2);     // Definita in particleLogic/compareParticle.c
Particle checkPos(int p1, int p2, Particle actPos[],Particle prePos);   // Definita in particleLogic/checkPos.c

#endif
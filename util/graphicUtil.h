/*
    Questo file contiene le definizioni delle costanti e delle funzioni utili per la
    rappresentazione grafica delle particelle e della griglia.
*/
#ifndef GRAPHICUTIL_H
#define GRAPHICUTIL_H

#define PARTICLE '.'    // Rappresentazione della particelle.
#define VOID ' '        // Rappresentazione della cella vuota.
#define CRISTAL '@'     // Rappresentazione della particelle cristallizzata.

#include <stdio.h>
#include "./particleLogic.h"

void printGrid(int dim, char* grid);    // Definita in graphicUtil/printGrid.c
void printParticles(Particle particles[], int numP, int dim);   // Definita in graphicUtil/printParticles.c
void gridToImage(Particle particle[], int numP, int dimG);  // Definita in graphicUtil/pgridToImage.c

#endif 
/*
    Questo file contiene la funzione per realizzare l'immagine del cristallo con la libreria stb_image

    INPUT:
        particles[] // Lista delle posizioni delle particelle.
        numP        // Numero di particelle nella griglia.
        dimG        // Dimensione della griglia.
*/
#include "../graphicUtil.h"

#define NCHANNEL 3      // Numero di canali RGB.
#define ALPHA 10        // Coefficiente per la sfumatura del colore.
#define PARTICLECH 70   // Colore delle particelle.
#define CRYSTALG 253    // Valore del canale G per il cristallo.
#define CRYSTALR 204    // Valore del canale R per il cristallo.

#define STB_IMAGE_IMPLEMENTATION
#include "../../stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../stb_image/stb_image_write.h"


void gridToImage(Particle particles[], int numP, int dimG)
{
    unsigned char *img = calloc(dimG*dimG*3, sizeof(char)); // Allocazione della memoria per l'immagine.
    Particle* seed = particles+numP-1;  // Puntatore al cristallo iniziale (che si trova nell'ultima posizione della lista).
    for (int p = 0; p < numP; p++)
    {
        int i = particles[p].x*dimG + particles[p].y;   // Indice relativo alla griglia della particella p.
        unsigned int dist = sqrt(pow(particles[p].x - seed->x, 2) + pow(particles[p].y - seed->y, 2));  // Calcolo della distanza dal cristallo iniziale.
        unsigned char c = 255 - dist*ALPHA;     // Calcolo del canale B per il cristallo.
        if (particles[p].stuck > 0) // Se la particella è cristallizzata utilizza i colori del cristallo.
        {
            img[i*3+0] = CRYSTALR %c;
            img[i*3+1] = CRYSTALG;
            img[i*3+2] = c;
        }
        else    // Se la particella non è cristallizzata il colore è grigio.
        {
            img[i*3+0] = PARTICLECH;
            img[i*3+1] = PARTICLECH;
            img[i*3+2] = PARTICLECH;
        }
        
    }
    stbi_write_png("./crystal.png", dimG, dimG, NCHANNEL, img, dimG*NCHANNEL);  // Scrive img in un file png.
}
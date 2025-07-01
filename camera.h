#include <stdio.h>
#include <SDL2/SDL.h>

#ifndef tCam
#define tCam

typedef struct _tCam{
    float pos[3];
    float centro[3];
    float cima[3];
    float **viewMatrix;
} tCamera3d;
#endif // tCam


//Aloca a estrutura de memória para representar a câmera
tCamera3d *criaCamera();

//Recebe os parâmetros específicos da câmera e calcula viewMatrix
void defineCamera(tCamera3d *camera, float posX, float posY, float posZ, float focX, float focY, float focZ, float cimX, float cimY, float cimZ);

//Desaloca a câmera
void desalocaCamera(tCamera3d *objeto);

/*#ifndef TRABALHO2_CG_CAMERA_H
#define TRABALHO2_CG_CAMERA_H

#endif //TRABALHO2_CG_CAMERA_H*/

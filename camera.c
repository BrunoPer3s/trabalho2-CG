#include <stdio.h>
#include <SDL2/SDL.h>
#include "camera.h"

//Aloca a estrutura de memória para representar a câmera
tCamera3d *criaCamera();

//Recebe os parâmetros específicos da câmera e calcula viewMatrix
void defineCamera(tCamera3d *camera, float posX, float posY, float posZ, float focX, float focY, float focZ, float cimX, float cimY, float cimZ);

//Desaloca a câmera
void desalocaCamera(tCamera3d *objeto);
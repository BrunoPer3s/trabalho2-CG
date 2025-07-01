#include <stdio.h>
#include <SDL2/SDL.h>
#include "camera.h"
#include <math.h>

//Aloca a estrutura de memória para representar a câmera
tCamera3d *criaCamera() {
    int i, j;
    tCamera3d *novacamera;

    novacamera = (tCamera3d *) malloc(sizeof(tCamera3d));

    novacamera->viewMatrix = (float **) malloc(sizeof(float *));

    for(i=0; i<4; i++){
        novacamera->viewMatrix[i] = (float *) malloc(sizeof(float));
    }

    novacamera->pos[0] = 0.0;
    novacamera->pos[0] = 0.0;
    novacamera->pos[0] = 1.0;

    novacamera->centro[0] = 0.0;
    novacamera->centro[0] = 0.0;
    novacamera->centro[0] = 0.0;

    novacamera->cima[0] = 0.0;
    novacamera->cima[0] = 1.0;
    novacamera->cima[0] = 0.0;

    for(i=0; i<4; i++){
        for(j=0; j<4; j++)
            if(i==j){
                novacamera->viewMatrix[i][j] = 1.0;
            }
            else if(i==2 && j==1) novacamera->viewMatrix[i][j] = 0.25;
            else if(i==1 && j==2) novacamera->viewMatrix[i][j] = -0.25;
            else if(i==1 && j==3) novacamera->viewMatrix[i][j] = -1.5;
            else if(i==2 && j==3) novacamera->viewMatrix[i][j] = -5.0;
            else novacamera->viewMatrix[i][j] = 0.0;
    }
    return novacamera;
};

//Recebe os parâmetros específicos da câmera e calcula viewMatrix
void defineCamera(tCamera3d *camera, float posX, float posY, float posZ, float focX, float focY, float focZ, float cimX, float cimY, float cimZ){

        // Atualiza os vetores
        camera->pos[0] = posX; camera->pos[1] = posY; camera->pos[2] = posZ;
        camera->centro[0] = focX; camera->centro[1] = focY; camera->centro[2] = focZ;
        camera->cima[0] = cimX; camera->cima[1] = cimY; camera->cima[2] = cimZ;

        // Calcula vetores direção
        float z[3], x[3], y[3];
        float norm;

        // z = normalize(pos - centro)
        z[0] = posX - focX;
        z[1] = posY - focY;
        z[2] = posZ - focZ;
        norm = sqrt(z[0]*z[0] + z[1]*z[1] + z[2]*z[2]);
        for (int i = 0; i < 3; i++) z[i] /= norm;

        // x = normalize(cross(cima, z))
        x[0] = cimY * z[2] - cimZ * z[1];
        x[1] = cimZ * z[0] - cimX * z[2];
        x[2] = cimX * z[1] - cimY * z[0];
        norm = sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
        for (int i = 0; i < 3; i++) x[i] /= norm;

        // y = cross(z, x)
        y[0] = z[1] * x[2] - z[2] * x[1];
        y[1] = z[2] * x[0] - z[0] * x[2];
        y[2] = z[0] * x[1] - z[1] * x[0];

        // Monta a viewMatrix
        for (int i = 0; i < 4; i++) {
            if (!camera->viewMatrix[i])
                camera->viewMatrix[i] = malloc(4 * sizeof(float));
        }

        camera->viewMatrix[0][0] = x[0];
        camera->viewMatrix[0][1] = x[1];
        camera->viewMatrix[0][2] = x[2];
        camera->viewMatrix[0][3] = - (x[0]*posX + x[1]*posY + x[2]*posZ);

        camera->viewMatrix[1][0] = y[0];
        camera->viewMatrix[1][1] = y[1];
        camera->viewMatrix[1][2] = y[2];
        camera->viewMatrix[1][3] = - (y[0]*posX + y[1]*posY + y[2]*posZ);

        camera->viewMatrix[2][0] = z[0];
        camera->viewMatrix[2][1] = z[1];
        camera->viewMatrix[2][2] = z[2];
        camera->viewMatrix[2][3] = - (z[0]*posX + z[1]*posY + z[2]*posZ);

        camera->viewMatrix[3][0] = 0.0f;
        camera->viewMatrix[3][1] = 0.0f;
        camera->viewMatrix[3][2] = 0.0f;
        camera->viewMatrix[3][3] = 1.0f;
}

//Desaloca a câmera
void desalocaCamera(tCamera3d *camera) {
    if (!camera) return;
    if (camera->viewMatrix) {
        for (int i = 0; i < 4; i++)
            free(camera->viewMatrix[i]);
        free(camera->viewMatrix);
    }
    free(camera);
}
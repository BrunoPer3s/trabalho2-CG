#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "objeto.h"
#include "algebra.h"

tObjeto3d *carregaObjeto(char *nomeArquivo) {
    printf("%s", nomeArquivo);
    FILE *fp = fopen(nomeArquivo, "r");
    if (!fp) {
        perror("Erro ao abrir o arquivo");
        return NULL;
    }

    tObjeto3d *obj = malloc(sizeof(tObjeto3d));
    if (!obj) {
        fclose(fp);
        perror("Erro ao alocar memória para o objeto");
        return NULL;
    }

    // Ler número de vértices
    fscanf(fp, "%d", &obj->nPontos);
    printf("\nNPontos: %d\n", obj->nPontos);


    // Alocar pontos
    obj->pontos = malloc(obj->nPontos * sizeof(float*));
    for (int i = 0; i < obj->nPontos; i++) {
        obj->pontos[i] = malloc(3 * sizeof(float));
        fscanf(fp, "%f %f %f", &obj->pontos[i][0], &obj->pontos[i][1], &obj->pontos[i][2]);
        printf("\nPontos: %f %f %f\n", obj->pontos[i][0], obj->pontos[i][1],obj->pontos[i][2]);

    }

    // Ler número de arestas
    fscanf(fp, "%d", &obj->nArestas);
    printf("\nNARESTA: %d\n", obj->nArestas);


    // Alocar arestas
    obj->arestas = malloc(obj->nArestas * sizeof(int*));
    for (int i = 0; i < obj->nArestas; i++) {
        obj->arestas[i] = malloc(2 * sizeof(int));
        fscanf(fp, "%d %d", &obj->arestas[i][0], &obj->arestas[i][1]);
    }

    // Inicializar transformações
    obj->transl = malloc(3 * sizeof(float));
    obj->escala = malloc(3 * sizeof(float));
    obj->transl[0] = obj->transl[1] = obj->transl[2] = 0.0f;
    obj->escala[0] = obj->escala[1] = obj->escala[2] = 1.0f;
    obj->Rx = obj->Ry = obj->Rz = 0.0f;


    // A matriz de modelo pode ser inicializada posteriormente
    //obj->modelMatrix = NULL;
    obj->modelMatrix = malloc(4 * sizeof(float*));
    for (int i = 0; i < 4; i++) {
        obj->modelMatrix[i] = malloc(4 * sizeof(float));
        for (int j = 0; j < 4; j++) {
            obj->modelMatrix[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }

    fclose(fp);
    return obj;
}


//Altera a modelMatrix de um objeto para redimenciona-lo segundo os parâmetros escalaX, escalaY e escalaZ
void transladaObjeto(tObjeto3d *objeto, float tx, float ty, float tz) {
    objeto->transl[0] = tx;
    objeto->transl[1] = ty;
    objeto->transl[2] = tz;
}

void escalaObjeto(tObjeto3d *objeto, float sx, float sy, float sz) {
    objeto->escala[0] = sx;
    objeto->escala[1] = sy;
    objeto->escala[2] = sz;
}

//Altera a modelMatrix de um objeto para rotaciona-lo ao redor do eixo X segundo o angulo informado

float **rotacionaObjetoEixoX(float angulo) {
    float rad = angulo * M_PI / 180.0f;
    float c = cosf(rad);
    float s = sinf(rad);

    float **Rx = criaMatrizIdentidade4d();

    Rx[1][1] = c;
    Rx[1][2] = -s;
    Rx[2][1] = s;
    Rx[2][2] = c;


    return Rx;
}

//Altera a modelMatrix de um objeto para rotaciona-lo ao redor do eixo Y segundo o angulo informado
float **rotacionaObjetoEixoY(float angulo) {
    float rad = angulo * M_PI / 180.0f;
    float c = cosf(rad);
    float s = sinf(rad);

    float **Ry = criaMatrizIdentidade4d();

    Ry[0][0] = c;
    Ry[0][2] = s;
    Ry[2][0] = -s;
    Ry[2][2] = c;

    return Ry;
}

//Altera a modelMatrix de um objeto para rotaciona-lo ao redor do eixo Z segundo o angulo informado
float **rotacionaObjetoEixoZ(float angulo) {
    float rad = angulo * M_PI / 180.0f;
    float c = cosf(rad);
    float s = sinf(rad);

    float **Rz = criaMatrizIdentidade4d();

    Rz[0][0] = c;
    Rz[0][1] = -s;
    Rz[1][0] = s;
    Rz[1][1] = c;

    return Rz;
}

//Imprime um objeto no terminal
void imprimeObjetoDBG(tObjeto3d *objeto){

}

//Desaloca o objeto
void desalocaObjeto(tObjeto3d *objeto){
    for (int i = 0; i < objeto->nPontos; i++) free(objeto->pontos[i]);
    for (int i = 0; i < objeto->nArestas; i++) free(objeto->arestas[i]);
    free(objeto->pontos);
    free(objeto->arestas);
    free(objeto->transl);
    free(objeto->escala);
    if (objeto->modelMatrix) {
        for (int i = 0; i < 4; i++) free(objeto->modelMatrix[i]);
        free(objeto->modelMatrix);
    }
    free(objeto);
}
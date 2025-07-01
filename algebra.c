#include <stdlib.h>
#include <stdio.h>

//Ajusta valores de um matriz 4d para uma matriz identidade
void criaIdentidade4d(float **novaMatriz){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            novaMatriz[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

float **criaMatrizIdentidade4d() {
    float **m = malloc(4 * sizeof(float *));
    for (int i = 0; i < 4; i++) {
        m[i] = malloc(4 * sizeof(float));
        for (int j = 0; j < 4; j++) {
            m[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
    return m;
}

void liberaMatriz4d(float **matriz) {
    for (int i = 0; i < 4; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

//Imprime uma matriz 4d no terminal
void imprimeMatriz4dDBG(float **matriz){
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%f ", matriz[i][j]);
        }
        printf("\n");
    }
}

//Multiplica uma matriz 4d por um ponto
float *multMatriz4dPonto(float **matriz, float *ponto) {
    float *resultado = (float *) malloc(4 * sizeof(float));

    for(int i = 0; i<4; i++){
        resultado[i] = 0.0;
        for(int j = 0; j<4; j++){
            resultado[i] += matriz[i][j] * ponto[j];
        }
    }
    return resultado;
}

//Multiplica duas matrizes 4d salvando o resultado na segunda matriz
void multMatriz4d(float **matrizA, float **matrizB) {
    float **temp = malloc(4 * sizeof(float*));
    for (int i = 0; i < 4; i++) {
        temp[i] = malloc(4 * sizeof(float));
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            temp[i][j] = 0.0f;
            for (int k = 0; k < 4; k++) {
                temp[i][j] += matrizA[i][k] * matrizB[k][j];
            }
        }
    }

    // Copia o resultado de volta para matrizB
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrizB[i][j] = temp[i][j];
        }
    }

    // Libera a matriz temporária
    for (int i = 0; i < 4; i++) {
        free(temp[i]);
    }
    free(temp);
}

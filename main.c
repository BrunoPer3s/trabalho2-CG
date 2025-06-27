#include <stdio.h>
#include <SDL2/SDL.h>
#include "objeto.h"
#include "tela.h"
#include "algebra.h"

//desenha um objeto na tela
void desenhaObjetoTela(SDL_Renderer *renderer, float **matriz, tObjeto3d *objeto){
    for (int i = 0; i < objeto->nArestas; i++) {
        int idx1 = objeto->arestas[i][0];
        int idx2 = objeto->arestas[i][1];

        float *p1 = objeto->pontos[idx1];
        float *p2 = objeto->pontos[idx2];

        float ponto1[4] = {p1[0], p1[1], p1[2], 1.0f};
        float ponto2[4] = {p2[0], p2[1], p2[2], 1.0f};

        float result1[4] = {0};
        float result2[4] = {0};

        // Multiplicação da matriz 4x4 com ponto1 e ponto2
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result1[i] += matriz[i][j] * ponto1[j];
                result2[i] += matriz[i][j] * ponto2[j];
            }
        }

        // Projeção ortográfica: usar X e Y apenas
        float ponto2d1[2] = { result1[0], result1[1] };
        float ponto2d2[2] = { result2[0], result2[1] };

        desenhaArestaTela(renderer, ponto2d1, ponto2d2);
    }
}

void atualizaModelMatrix(tObjeto3d *objeto) {
    // Alocar matriz identidade
    if (objeto->modelMatrix == NULL) {
        objeto->modelMatrix = malloc(4 * sizeof(float*));
        for (int i = 0; i < 4; i++)
            objeto->modelMatrix[i] = malloc(4 * sizeof(float));
    }

    criaIdentidade4d(objeto->modelMatrix);

    // Cria matriz de escala
    float **S = criaMatrizIdentidade4d();
    S[0][0] = objeto->escala[0];
    S[1][1] = objeto->escala[1];
    S[2][2] = objeto->escala[2];

    // Cria rotações
    float **Rx = rotacionaObjetoEixoX(objeto->Rx);
    float **Ry = rotacionaObjetoEixoY(objeto->Ry);
    float **Rz = rotacionaObjetoEixoZ(objeto->Rz);

    // Cria translação
    float **T = criaMatrizIdentidade4d();
    T[0][3] = objeto->transl[0];
    T[1][3] = objeto->transl[1];
    T[2][3] = objeto->transl[2];

    // Combina todas: M = T * Rz * Ry * Rx * S
    multMatriz4d(Rx, S);      // S = Rx * S
    multMatriz4d(S, Ry);     // S = Ry * Rx * S
    multMatriz4d(Ry,Rz);     // S = Rz * Ry * Rx * S
    multMatriz4d(Rz, T);      // S = T * Rz * Ry * Rx * S

    // Copia resultado final em modelMatrix
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            objeto->modelMatrix[i][j] = T[i][j];

    // Liberar temporários
    liberaMatriz4d(S);
    liberaMatriz4d(Rx);
    liberaMatriz4d(Ry);
    liberaMatriz4d(Rz);
    liberaMatriz4d(T);
}

int main( int argc, char * argv[] ){
    if (SDL_Init( SDL_INIT_EVERYTHING) < 0){
        printf("SDL não inicializou! SDL Erro: %s\n", SDL_GetError());
    }

    SDL_Window *window = criaTela("Hello SDL World!");

    if(window == NULL){
        printf("SDL não criou a janela! SDL Erro: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Event windowEvent;

    tObjeto3d *meuObjeto = carregaObjeto("cubo.txt");
    escalaObjeto(meuObjeto, 1.0f, 1.0f, 1.0f); // Faz caber na tela

    if (meuObjeto) {
        printf("Objeto carregado com %d vértices e %d arestas\n", meuObjeto->nPontos, meuObjeto->nArestas);
        criaIdentidade4d(meuObjeto->modelMatrix);
    }

    while(1){
        if( SDL_PollEvent(&windowEvent)){
            if(windowEvent.type == SDL_QUIT){
                break;
            }
        }
        if (windowEvent.type == SDL_KEYDOWN) {
            // ROTAÇÕES
            if (windowEvent.key.keysym.sym == SDLK_q) {
                meuObjeto->Rx += 0.5f;
                atualizaModelMatrix(meuObjeto);
            }
            if (windowEvent.key.keysym.sym == SDLK_e) {
                meuObjeto->Rx -= 0.5f;
                atualizaModelMatrix(meuObjeto);
            }
            if (windowEvent.key.keysym.sym == SDLK_a) {
                meuObjeto->Ry += 0.5f;
                atualizaModelMatrix(meuObjeto);
            }
            if (windowEvent.key.keysym.sym == SDLK_d) {
                meuObjeto->Ry -= 0.5f;
                atualizaModelMatrix(meuObjeto);
            }
            if (windowEvent.key.keysym.sym == SDLK_z) {
                meuObjeto->Rz += 0.5f;
                atualizaModelMatrix(meuObjeto);
            }
            if (windowEvent.key.keysym.sym == SDLK_c) {
                meuObjeto->Rz -= 0.5f;
                atualizaModelMatrix(meuObjeto);
            }

            // TRANSLAÇÃO
            if (windowEvent.key.keysym.sym == SDLK_UP) {
                meuObjeto->transl[1] += 0.1f;
                atualizaModelMatrix(meuObjeto);
            }
            if (windowEvent.key.keysym.sym == SDLK_DOWN) {
                meuObjeto->transl[1] -= 0.1f;
                atualizaModelMatrix(meuObjeto);
            }
            if (windowEvent.key.keysym.sym == SDLK_LEFT) {
                meuObjeto->transl[0] -= 0.1f;
                atualizaModelMatrix(meuObjeto);
            }
            if (windowEvent.key.keysym.sym == SDLK_RIGHT) {
                meuObjeto->transl[0] += 0.1f;
                atualizaModelMatrix(meuObjeto);
            }

            // ESCALA
            if (windowEvent.key.keysym.sym == SDLK_EQUALS) {
                meuObjeto->escala[0] *= 1.1f;
                meuObjeto->escala[1] *= 1.1f;
                meuObjeto->escala[2] *= 1.1f;
                atualizaModelMatrix(meuObjeto);
            }

            if (windowEvent.key.keysym.sym == SDLK_MINUS) {
                meuObjeto->escala[0] *= 0.9f;
                meuObjeto->escala[1] *= 0.9f;
                meuObjeto->escala[2] *= 0.9f;
                atualizaModelMatrix(meuObjeto);
            }
        }

        SDL_SetRenderDrawColor(renderer, 242, 242, 242, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        desenhaObjetoTela(renderer,meuObjeto->modelMatrix,meuObjeto);

        SDL_RenderPresent(renderer);
        }

    desalocaObjeto(meuObjeto);
    desalocaTela(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

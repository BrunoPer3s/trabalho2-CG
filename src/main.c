#include <stdio.h>
#include <SDL2/SDL.h>
#include "objeto.h"
#include "tela.h"
#include "algebra.h"
#include "camera.h"
#include <math.h>

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

void doFlip(float **matrizA, float theta, float valor){
    matrizA[1][1] = cos(theta);
    matrizA[1][2] = -sin(theta);
    matrizA[2][1] = sin(theta);
    matrizA[2][2] = cos(theta);

    matrizA[0][3] = 1.0f;
    matrizA[1][3] = valor;
    matrizA[2][3] = 1.0f;
}

int main( int argc, char * argv[] ){
    float **matrizFlip = criaMatrizIdentidade4d();
    float anguloFlip = 0.0f;
    int flipLigado = 1;  // começa com doFlip ativo
    float translFlip[3] = {0.0f, 0.0f, 0.0f};  // controle de translação do objeto 2

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

    tObjeto3d *meuObjeto = carregaObjeto("piramide.txt");
    escalaObjeto(meuObjeto, 1.0f, 1.0f, 1.0f); // Faz caber na tela

    //tObjeto3d *objetoFlip = carregaObjeto("cubo.txt");
    //escalaObjeto(objetoFlip, 1.0f, 1.0f, 1.0f);


    if (meuObjeto) {
        printf("Objeto carregado com %d vértices e %d arestas\n", meuObjeto->nPontos, meuObjeto->nArestas);
        criaIdentidade4d(meuObjeto->modelMatrix);
    }

    tCamera3d *camera = criaCamera();
    defineCamera(camera, 0.0f, 0.0f, 10.0f,   // posição da câmera
                        0.0f, 0.0f, 0.0f,    // centro (olhando para a origem)
                        0.0f, 1.0f, 0.0f);   // vetor cima

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
            if (windowEvent.key.keysym.sym == SDLK_i) {
                translFlip[1] += 0.1f;  // cima
            }
            if (windowEvent.key.keysym.sym == SDLK_k) {
                translFlip[1] -= 0.1f;  // baixo
            }
            if (windowEvent.key.keysym.sym == SDLK_j) {
                translFlip[0] -= 0.1f;  // esquerda
            }
            if (windowEvent.key.keysym.sym == SDLK_l) {
                translFlip[0] += 0.1f;  // direita
            }

            // Toggle doFlip com tecla P
            if (windowEvent.key.keysym.sym == SDLK_p) {
                flipLigado = !flipLigado;
            }

            // ESCALA
            if (windowEvent.key.keysym.sym == SDLK_EQUALS) {
                for (int i = 0; i < 3; i++) {
                    meuObjeto->escala[i] *= 1.1f;
                    //objetoFlip->escala[i] *= 1.1f;
                }
                atualizaModelMatrix(meuObjeto);
                //atualizaModelMatrix(objetoFlip);
            }
            if (windowEvent.key.keysym.sym == SDLK_MINUS) {
                for (int i = 0; i < 3; i++) {
                    meuObjeto->escala[i] *= 0.9f;
                    //objetoFlip->escala[i] *= 0.9f;
                }
                atualizaModelMatrix(meuObjeto);
                //atualizaModelMatrix(objetoFlip);
            }
            // Numpad controla posição da câmera
            if (windowEvent.key.keysym.sym == SDLK_KP_4) {  // Esquerda
                camera->pos[0] -= 0.5f;
            }
            if (windowEvent.key.keysym.sym == SDLK_KP_6) {  // Direita
                camera->pos[0] += 0.5f;
            }
            if (windowEvent.key.keysym.sym == SDLK_KP_8) {  // Cima
                camera->pos[1] += 0.5f;
            }
            if (windowEvent.key.keysym.sym == SDLK_KP_2) {  // Baixo
                camera->pos[1] -= 0.5f;
            }
            if (windowEvent.key.keysym.sym == SDLK_KP_7) {  // Zoom out
                camera->pos[2] += 0.5f;
            }
            if (windowEvent.key.keysym.sym == SDLK_KP_9) {  // Zoom in
                camera->pos[2] -= 0.5f;
            }
            if (windowEvent.key.keysym.sym == SDLK_KP_1) {
                camera->centro[0] -= 0.5f;  // olha mais para a esquerda
            }
            if (windowEvent.key.keysym.sym == SDLK_KP_3) {
                camera->centro[0] += 0.5f;  // olha mais para a direita
            }
            // Atualiza viewMatrix após movimentar a câmera
            defineCamera(camera,
                         camera->pos[0], camera->pos[1], camera->pos[2],
                         camera->centro[0], camera->centro[1], camera->centro[2],
                         camera->cima[0], camera->cima[1], camera->cima[2]);
        }

        anguloFlip += 0.01f;
        criaIdentidade4d(matrizFlip);  // Zera antes de aplicar

        if (flipLigado) {
            doFlip(matrizFlip, anguloFlip, 2.0f);
        }
        // Aplica translação manual
        matrizFlip[0][3] += translFlip[0];
        matrizFlip[1][3] += translFlip[1];
        matrizFlip[2][3] += translFlip[2];

        SDL_SetRenderDrawColor(renderer, 242, 242, 242, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        float **MV = criaMatrizIdentidade4d();
        multMatriz4d(meuObjeto->modelMatrix, MV);
        multMatriz4d(camera->viewMatrix, MV);
        desenhaObjetoTela(renderer, MV, meuObjeto);
        liberaMatriz4d(MV);

        float **MV2 = criaMatrizIdentidade4d();
        //multMatriz4d(objetoFlip->modelMatrix, MV2);  // aplicar escala
        multMatriz4d(matrizFlip, MV2);
        //desenhaObjetoTela(renderer, MV2, objetoFlip);
        liberaMatriz4d(MV2);

        SDL_RenderPresent(renderer);
        }

    desalocaObjeto(meuObjeto);
    desalocaTela(window);
    desalocaCamera(camera);
    //desalocaObjeto(objetoFlip);
    liberaMatriz4d(matrizFlip);
    SDL_Quit();

    return EXIT_SUCCESS;
}

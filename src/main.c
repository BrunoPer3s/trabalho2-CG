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

void atualizaOrthoMatrix(float **orthoMatrix, float escala) {
    criaIdentidade4d(orthoMatrix);
    orthoMatrix[0][0] = 1.0f / escala;
    orthoMatrix[1][1] = 1.0f / escala;
    orthoMatrix[2][2] = 1.0f / escala; // pode ser 1.0f se você quiser ignorar Z
}

int main( int argc, char * argv[] ){
    float escalaOrtho = 1.0f;  // escala da projeção ortográfica (zoom)
    float **orthoMatrix = criaMatrizIdentidade4d();
    atualizaOrthoMatrix(orthoMatrix, escalaOrtho);

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

    tObjeto3d *objeto1 = carregaObjeto("piramide.txt");

    objeto1->escala[0] *= 0.05f;
    objeto1->escala[1] *= 0.05f;
    objeto1->escala[2] *= 0.05f;
    //escalaObjeto(objeto1, 0.25f, 0.25f, 0.25f); // Faz caber na tela

    if (objeto1) {
        printf("\nObjeto carregado com %d vértices e %d arestas\n", objeto1->nPontos, objeto1->nArestas);
        criaIdentidade4d(objeto1->modelMatrix);
    }

    tCamera3d *camera = criaCamera();
    defineCamera(camera, 0.0f, 0.0f, 5.0f,   // posição da câmera
                        0.0f, 0.0f, 0.0f,    // centro (olhando para a origem)
                        0.0f, 1.0f, 0.0f);
                        
                           // vetor cima

    while(1){
        if( SDL_PollEvent(&windowEvent)){
            if(windowEvent.type == SDL_QUIT){
                break;
            }
        }
        if (windowEvent.type == SDL_KEYDOWN) {
            // Interações com o Objeto
            // ROTAÇÕES
            if (windowEvent.key.keysym.sym == SDLK_q) {
                objeto1->Rx += 0.5f;
            }
            if (windowEvent.key.keysym.sym == SDLK_e) {
                objeto1->Rx -= 0.5f;
            }
            if (windowEvent.key.keysym.sym == SDLK_a) {
                objeto1->Ry += 0.5f;
            }
            if (windowEvent.key.keysym.sym == SDLK_d) {
                objeto1->Ry -= 0.5f;
            }
            if (windowEvent.key.keysym.sym == SDLK_z) {
                objeto1->Rz += 0.5f;
            }
            if (windowEvent.key.keysym.sym == SDLK_c) {
                objeto1->Rz -= 0.5f;
            }

            // TRANSLAÇÃO
            if (windowEvent.key.keysym.sym == SDLK_UP) {
                objeto1->transl[1] += 0.1f;
            }
            if (windowEvent.key.keysym.sym == SDLK_DOWN) {
                objeto1->transl[1] -= 0.1f;
            }
            if (windowEvent.key.keysym.sym == SDLK_LEFT) {
                objeto1->transl[0] -= 0.1f;
            }
            if (windowEvent.key.keysym.sym == SDLK_RIGHT) {
                objeto1->transl[0] += 0.1f;
            }

            // ESCALA
            if (windowEvent.key.keysym.sym == SDLK_EQUALS) {
                for (int i = 0; i < 3; i++) {
                    objeto1->escala[i] *= 1.1f;
                }
            }
            if (windowEvent.key.keysym.sym == SDLK_MINUS) {
                for (int i = 0; i < 3; i++) {
                    objeto1->escala[i] *= 0.9f;
                }
            }
            // Interações com a câmera, Numpad
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
                camera->centro[1] += 0.5f;
            }
            if (windowEvent.key.keysym.sym == SDLK_KP_9) {  // Zoom in
                camera->centro[1] -= 0.5f;
            }
            if (windowEvent.key.keysym.sym == SDLK_KP_1) {
                camera->centro[0] -= 0.5f;  // olha mais para a esquerda
            }
            if (windowEvent.key.keysym.sym == SDLK_KP_3) {
                camera->centro[0] += 0.5f;  // olha mais para a direita
            }

             // Reseta a Câmera
            if (windowEvent.key.keysym.sym == SDLK_b) {
                defineCamera(camera, 0.0f, 0.0f, 10.0f,   // posição da câmera
                        0.0f, 0.0f, 0.0f,    // centro (olhando para a origem)
                        0.0f, 1.0f, 0.0f);   // vetor cimaobjeto1->Rx += 0.5f;
            }

            //Interações Câmera Projecão
            if (windowEvent.key.keysym.sym == SDLK_EQUALS && (windowEvent.key.keysym.mod & KMOD_LCTRL)) {
            // Zoom in (CTRL + =)
                escalaOrtho *= 0.9f;
                if (escalaOrtho < 0.1f) escalaOrtho = 0.1f;
                atualizaOrthoMatrix(orthoMatrix, escalaOrtho);
            }

            if (windowEvent.key.keysym.sym == SDLK_MINUS && (windowEvent.key.keysym.mod & KMOD_LCTRL)) {
                // Zoom out (CTRL + -)
                escalaOrtho *= 1.1f;
                if (escalaOrtho > 100.0f) escalaOrtho = 100.0f;
                atualizaOrthoMatrix(orthoMatrix, escalaOrtho);
            }
            // Atualiza viewMatrix após movimentar a câmera
            defineCamera(camera,
                         camera->pos[0], camera->pos[1], camera->pos[2],
                         camera->centro[0], camera->centro[1], camera->centro[2],
                         camera->cima[0], camera->cima[1], camera->cima[2]);
        }

        SDL_SetRenderDrawColor(renderer, 242, 242, 242, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    
        atualizaModelMatrix(objeto1); // Atualiza a modelMatrix do objeto

        float **MV = criaMatrizIdentidade4d();
        multMatriz4d(objeto1->modelMatrix, MV);
        multMatriz4d(camera->viewMatrix, MV);
        multMatriz4d(orthoMatrix, MV); 
        desenhaObjetoTela(renderer, MV, objeto1);
        liberaMatriz4d(MV);

        SDL_RenderPresent(renderer);
        }

    desalocaObjeto(objeto1);
    desalocaTela(window);
    desalocaCamera(camera);
    SDL_Quit();

    return EXIT_SUCCESS;
}
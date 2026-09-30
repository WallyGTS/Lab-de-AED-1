/*
 * Ponteiros + Alocação Dinâmica (vetor e matriz) com raylib
 * ---------------------------------------------------------------
 * Este programa desenha:
 *   1) Uma matriz dinâmica (grade de células coloridas) alocada com malloc,
 *      onde cada linha é um ponteiro para um vetor de inteiros.
 *   2) Um vetor dinâmico de bolinhas (struct Bola) que se movem na tela,
 *      manipuladas via ponteiros.
 */

#include "raylib.h"
#include <stdlib.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA  600
#define TAM_CELULA     40   // tamanho de cada célula da grade (matriz)

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float   raio;
    Color   cor;
} Bola;

/* cria uma MATRIZ dinâmica de inteiros (linhas x colunas) */
int **criarMatriz(int linhas, int colunas) {
    int **matriz = (int **)malloc(linhas * sizeof(int *));
    if (matriz == NULL) return NULL;

    for (int i = 0; i < linhas; i++) {
        matriz[i] = (int *)malloc(colunas * sizeof(int));
        for (int j = 0; j < colunas; j++) {
            matriz[i][j] = GetRandomValue(0, 1);
        }
    }
    return matriz;
}

/* libera a memória da matriz */
void liberarMatriz(int **matriz, int linhas) {
    for (int i = 0; i < linhas; i++) {
        free(matriz[i]);   // libera cada linha
    }
    free(matriz);          // libera o vetor de ponteiros
}

/* desenha a matriz na tela */
void desenharMatriz(int **matriz, int linhas, int colunas) {
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            Color cor = (matriz[i][j] == 1) ? (Color){20, 40, 70, 255}
                                            : (Color){15, 30, 55, 255};
            DrawRectangle(j * TAM_CELULA, i * TAM_CELULA,
                           TAM_CELULA - 2, TAM_CELULA - 2, cor);
        }
    }
}

/* cria o vetor dinâmico de bolas */
Bola *criarBolas(int quantidade) {
    Bola *bolas = (Bola *)malloc(quantidade * sizeof(Bola));
    if (bolas == NULL) return NULL;

    for (int i = 0; i < quantidade; i++) {
        Bola *b = (bolas + i);
        b->pos = (Vector2){ (float)GetRandomValue(50, LARGURA_JANELA - 50),
                            (float)GetRandomValue(50, ALTURA_JANELA - 50) };
        b->vel = (Vector2){ (float)GetRandomValue(-4, 4),
                            (float)GetRandomValue(-4, 4) };
        b->raio = (float)GetRandomValue(10, 25);
        b->cor  = (Color){ GetRandomValue(100,255), GetRandomValue(100,255),
                           GetRandomValue(100,255), 255 };
    }
    return bolas;
}

/* atualiza a posição de UMA bola via ponteiro */
void atualizarBola(Bola *b) {
    b->pos.x += b->vel.x;
    b->pos.y += b->vel.y;

    if (b->pos.x - b->raio < 0 || b->pos.x + b->raio > LARGURA_JANELA)
        b->vel.x *= -1;
    if (b->pos.y - b->raio < 0 || b->pos.y + b->raio > ALTURA_JANELA)
        b->vel.y *= -1;
}

int main(void) {
    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "Ponteiros e Alocacao Dinamica - raylib");
    SetTargetFPS(60);

    int linhas   = ALTURA_JANELA / TAM_CELULA;
    int colunas  = LARGURA_JANELA / TAM_CELULA;
    int **grade  = criarMatriz(linhas, colunas);

    int quantidadeBolas = 12;
    Bola *bolas = criarBolas(quantidadeBolas);

    while (!WindowShouldClose()) {
        for (int i = 0; i < quantidadeBolas; i++) {
            atualizarBola(bolas + i);
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            desenharMatriz(grade, linhas, colunas);

            for (int i = 0; i < quantidadeBolas; i++) {
                DrawCircleV(bolas[i].pos, bolas[i].raio, bolas[i].cor);
            }

            DrawText("Matriz (int**) e vetor de structs (Bola*) alocados com malloc",
                     10, 10, 18, WHITE);
            DrawText("Pressione ESC para sair", 10, ALTURA_JANELA - 25, 16, WHITE);
        EndDrawing();
    }

    free(bolas);
    liberarMatriz(grade, linhas);
    CloseWindow();
    
    return 0;
}
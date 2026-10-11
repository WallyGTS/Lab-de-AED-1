#include "raylib.h"
#include "entidade.h"
#include <stdlib.h>

int main(void) {
    InitWindow(800, 600, "Atividade 8b - Reuso de Modulos");
    SetTargetFPS(60);

    Entidade *jogador = entidadeCriar(ENTIDADE_JOGADOR, (Vector2){ 200, 300 });
    Entidade *inimigo = entidadeCriar(ENTIDADE_INIMIGO, (Vector2){ 400, 300 });
    Entidade *item = entidadeCriar(ENTIDADE_ITEM, (Vector2){ 600, 300 });

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);

            entidadeDesenhar(jogador);
            entidadeDesenhar(inimigo);
            entidadeDesenhar(item);

            DrawText("Reuso do modulo entidade.h e entidade.c", 180, 50, 20, DARKGRAY);
            DrawText("ESC para sair", 10, 570, 16, GRAY);

        EndDrawing();
    }

    free(jogador);
    free(inimigo);
    free(item);

    CloseWindow();
    return 0;
}
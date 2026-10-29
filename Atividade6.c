#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <raylib.h>

#define TELA_LARGURA     800
#define TELA_ALTURA      600
#define PLAYER_RAIO      20.0f
#define LIMITE_ENTIDADES 30
#define QTD_INIMIGOS     5
#define QTD_ITENS        6
#define ARQ_PLACAR      "placar.txt"
#define ARQ_SAVE        "save.bin"

typedef enum {
    CAT_PLAYER,
    CAT_INIMIGO,
    CAT_ITEM
} Categoria;

typedef union {
    int dano;
    int valor;
} DadosExtra;

typedef struct {
    Categoria tipo;
    Vector2 pos;
    float raio;
    int vida;
    Color cor;
    DadosExtra extra;
} ObjetoJogo;

ObjetoJogo *objetos[LIMITE_ENTIDADES];
int qtdObjetos = 0;

ObjetoJogo *gerarObjeto(Categoria tipo, Vector2 pos) {
    ObjetoJogo *e = (ObjetoJogo *)malloc(sizeof(ObjetoJogo));
    if (e == NULL) return NULL;

    e->tipo = tipo;
    e->pos = pos;
    e->raio = (tipo == CAT_PLAYER) ? PLAYER_RAIO
            : (tipo == CAT_INIMIGO) ? 15.0f
            : 8.0f;

    switch (tipo) {
        case CAT_PLAYER:
            e->vida = 100;
            e->cor = BLUE;
            break;
        case CAT_INIMIGO:
            e->vida = 40;
            e->cor = MAROON;
            e->extra.dano = GetRandomValue(5, 15);
            break;
        case CAT_ITEM:
            e->vida = 1;
            e->cor = GOLD;
            e->extra.valor = GetRandomValue(5, 20);
            break;
    }
    return e;
}

void registrarObjeto(ObjetoJogo *e) {
    if (e == NULL || qtdObjetos >= LIMITE_ENTIDADES) return;
    objetos[qtdObjetos] = e;
    qtdObjetos++;
}

void excluirObjeto(int indice) {
    if (indice < 0 || indice >= qtdObjetos) return;
    free(objetos[indice]);
    objetos[indice] = objetos[qtdObjetos - 1];
    objetos[qtdObjetos - 1] = NULL;
    qtdObjetos--;
}

void limparObjetos(void) {
    for (int i = 0; i < qtdObjetos; i++) {
        free(objetos[i]);
        objetos[i] = NULL;
    }
    qtdObjetos = 0;
}

bool houveColisao(ObjetoJogo *a, ObjetoJogo *b) {
    float dx = a->pos.x - b->pos.x;
    float dy = a->pos.y - b->pos.y;
    float distancia = sqrtf(dx * dx + dy * dy);
    return distancia <= (a->raio + b->raio);
}

void renderizarObjeto(ObjetoJogo *e) {
    DrawCircleV(e->pos, e->raio, e->cor);
    if (e->tipo == CAT_INIMIGO) {
        DrawText(TextFormat("%d", e->vida), (int)e->pos.x - 8, (int)e->pos.y - 26, 14, BLACK);
    }
}

void registrarPlacar(char nomePlayer[], int pontos) {
    FILE *arquivo = fopen(ARQ_PLACAR, "a");
    if (arquivo == NULL) return;
    fprintf(arquivo, "%s %d\n", nomePlayer, pontos);
    fclose(arquivo);
}

int buscarRecorde(void) {
    FILE *arquivo = fopen(ARQ_PLACAR, "r");
    if (arquivo == NULL) return 0;

    char nomeLido[16];
    int valor = 0;
    int melhor = 0;

    while (fscanf(arquivo, "%15s %d", nomeLido, &valor) == 2) {
        if (valor > melhor) melhor = valor;
    }
    fclose(arquivo);
    return melhor;
}

bool gravarSave(void) {
    FILE *arquivo = fopen(ARQ_SAVE, "wb");
    if (arquivo == NULL) return false;

    fwrite(&qtdObjetos, sizeof(int), 1, arquivo);
    for (int i = 0; i < qtdObjetos; i++) {
        fwrite(objetos[i], sizeof(ObjetoJogo), 1, arquivo);
    }
    fclose(arquivo);
    return true;
}

bool abrirSave(void) {
    FILE *arquivo = fopen(ARQ_SAVE, "rb");
    if (arquivo == NULL) return false;

    int totalSalvo = 0;
    if (fread(&totalSalvo, sizeof(int), 1, arquivo) != 1) {
        fclose(arquivo);
        return false;
    }

    limparObjetos();

    for (int i = 0; i < totalSalvo; i++) {
        ObjetoJogo *e = (ObjetoJogo *)malloc(sizeof(ObjetoJogo));
        if (e == NULL) {
            fclose(arquivo);
            return false;
        }

        if (fread(e, sizeof(ObjetoJogo), 1, arquivo) != 1) {
            free(e);
            break;
        }
        registrarObjeto(e);
    }

    fclose(arquivo);
    return true;
}

int main(void) {
    srand((unsigned int)time(NULL));

    char nomePlayer[16];
    printf("Digite o nome do player: ");
    scanf("%15s", nomePlayer);

    InitWindow(TELA_LARGURA, TELA_ALTURA, "Arena Foxy - Sistema de Saves");
    SetTargetFPS(60);

    ObjetoJogo *player = gerarObjeto(CAT_PLAYER, (Vector2){ TELA_LARGURA / 2.0f, TELA_ALTURA / 2.0f });
    registrarObjeto(player);

    for (int i = 0; i < QTD_INIMIGOS; i++) {
        Vector2 pos = { (float)GetRandomValue(30, TELA_LARGURA - 30), (float)GetRandomValue(30, TELA_ALTURA - 30) };
        registrarObjeto(gerarObjeto(CAT_INIMIGO, pos));
    }

    for (int i = 0; i < QTD_ITENS; i++) {
        Vector2 pos = { (float)GetRandomValue(30, TELA_LARGURA - 30), (float)GetRandomValue(30, TELA_ALTURA - 30) };
        registrarObjeto(gerarObjeto(CAT_ITEM, pos));
    }

    int pontos = 0;
    int recorde = buscarRecorde();
    char aviso[64] = "";
    float tempoAviso = 0.0f;

    while (!WindowShouldClose()) {
        float vel = 250.0f * GetFrameTime();

        // Movimentação com limite para não sair da tela
        if (player != NULL) {
            if (IsKeyDown(KEY_RIGHT) && player->pos.x < TELA_LARGURA - player->raio) player->pos.x += vel;
            if (IsKeyDown(KEY_LEFT)  && player->pos.x > player->raio) player->pos.x -= vel;
            if (IsKeyDown(KEY_UP)    && player->pos.y > player->raio) player->pos.y -= vel;
            if (IsKeyDown(KEY_DOWN)  && player->pos.y < TELA_ALTURA - player->raio) player->pos.y += vel;
        }

        for (int i = 1; i < qtdObjetos; i++) {
            ObjetoJogo *e = objetos[i];
            if (!houveColisao(player, e)) continue;

            if (e->tipo == CAT_ITEM) {
                pontos += e->extra.valor;
                excluirObjeto(i);
                i--;
            } else if (e->tipo == CAT_INIMIGO) {
                player->vida -= e->extra.dano;
                if (player->vida < 0) player->vida = 0;
            }
        }

        if (IsKeyPressed(KEY_F5)) {
            registrarPlacar(nomePlayer, pontos);
            if (pontos > recorde) recorde = pontos;
            TextCopy(aviso, "Placar salvo em placar.txt!");
            tempoAviso = 2.0f;
        }

        if (IsKeyPressed(KEY_F6)) {
            bool ok = gravarSave();
            TextCopy(aviso, ok ? "Jogo salvo em save.bin!" : "Erro ao salvar save.bin!");
            tempoAviso = 2.0f;
        }

        if (IsKeyPressed(KEY_F9)) {
            bool ok = abrirSave();
            if (ok && qtdObjetos > 0) {
                player = objetos[0]; // Atualiza o ponteiro do player após carregar
            }
            TextCopy(aviso, ok ? "Jogo carregado de save.bin!" : "Nenhum save.bin encontrado!");
            tempoAviso = 2.0f;
        }

        if (IsKeyPressed(KEY_DELETE)) {
            if (remove(ARQ_SAVE) == 0) {
                TextCopy(aviso, "save.bin apagado com sucesso!");
            } else {
                TextCopy(aviso, "Nenhum save encontrado.");
            }
            tempoAviso = 2.0f;
        }

        if (tempoAviso > 0.0f)
            tempoAviso -= GetFrameTime();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < qtdObjetos; i++) {
            renderizarObjeto(objetos[i]);
        }

        DrawText(TextFormat("Jogador: %s    Vida: %d    Pontuacao: %d    Recorde: %d", 
                 nomePlayer, player ? player->vida : 0, pontos, recorde), 10, 10, 20, DARKGRAY);
        DrawText("F5 salva placar (texto) | F6 salva jogo (binario) | F9 carrega jogo", 10, 36, 17, GRAY);
        DrawText("DELETE apaga save.bin | Setas movem | ESC sai", 10, TELA_ALTURA - 25, 16, GRAY);

        if (tempoAviso > 0.0f) {
            DrawText(aviso, 10, 62, 20, DARKGREEN);
        }

        EndDrawing();
    }

    limparObjetos();
    CloseWindow();
    return 0;
}

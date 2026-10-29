#include "entidade.h"
#include <stdlib.h>

Entidade* entidadeCriar(TipoEntidade tipo, Vector2 pos) {
    Entidade *e = (Entidade *)malloc(sizeof(Entidade));
    if (e == NULL) return NULL;

    e->tipo = tipo;
    e->pos = pos;

    switch (tipo) {
        case ENTIDADE_JOGADOR:
            e->vida = 100;
            e->largura = 30;
            e->altura = 30;
            e->extra.valor = 0;
            break;
        case ENTIDADE_INIMIGO:
            e->vida = 60;
            e->largura = 30;
            e->altura = 30;
            e->extra.dano = 1;      // dano aplicado a cada frame de colisão
            break;
        case ENTIDADE_ITEM:
            e->vida = 1;
            e->largura = 15;
            e->altura = 15;
            e->extra.valor = 10;    // pontos ao coletar
            break;
    }
    return e;
}

bool entidadeColidiu(Entidade *e1, Entidade *e2) {
    if (!e1 || !e2) return false;

    Rectangle r1 = { e1->pos.x, e1->pos.y, e1->largura, e1->altura };
    Rectangle r2 = { e2->pos.x, e2->pos.y, e2->largura, e2->altura };

    return CheckCollisionRecs(r1, r2);
}

void entidadeAplicarDano(Entidade *e, int dano) {
    if (e == NULL) return;
    e->vida -= dano;
    if (e->vida < 0) e->vida = 0;
}

void entidadeDesenhar(Entidade *e) {
    if (e == NULL) return;

    switch (e->tipo) {
        case ENTIDADE_JOGADOR:
            DrawRectangle((int)e->pos.x, (int)e->pos.y, e->largura, e->altura, SKYBLUE);
            DrawRectangleLines((int)e->pos.x, (int)e->pos.y, e->largura, e->altura, DARKBLUE);
            break;
        case ENTIDADE_INIMIGO:
            DrawRectangle((int)e->pos.x, (int)e->pos.y, e->largura, e->altura, RED);
            DrawRectangleLines((int)e->pos.x, (int)e->pos.y, e->largura, e->altura, MAROON);
            break;
        case ENTIDADE_ITEM:
            DrawCircle((int)(e->pos.x + e->largura / 2.0f), (int)(e->pos.y + e->altura / 2.0f),
                       e->largura / 2.0f, GOLD);
            break;
    }
}

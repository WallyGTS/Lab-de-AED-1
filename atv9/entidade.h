#ifndef ENTIDADE_H
#define ENTIDADE_H

#include "raylib.h"
#include <stdbool.h>

typedef enum {
    ENTIDADE_JOGADOR,
    ENTIDADE_INIMIGO,
    ENTIDADE_ITEM
} TipoEntidade;

typedef struct {
    TipoEntidade tipo;
    Vector2 pos;
    int vida;
    int largura;
    int altura;
} Entidade;

bool entidadeColidiu(Entidade *e1, Entidade *e2);

#endif

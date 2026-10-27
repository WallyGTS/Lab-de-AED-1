#include "entidade.h"

bool entidadeColidiu(Entidade *e1, Entidade *e2) {
    if (!e1 || !e2) return false;
    
    Rectangle r1 = { e1->pos.x, e1->pos.y, e1->largura, e1->altura };
    Rectangle r2 = { e2->pos.x, e2->pos.y, e2->largura, e2->altura };
    
    return CheckCollisionRecs(r1, r2);
}

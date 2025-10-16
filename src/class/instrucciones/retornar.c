#include "retornar.h"
#include <stdio.h>

TipoRetorno ejecutarRetornar(struct ASTNode* node, struct entorno* e) {
    TipoRetorno res = { NULL, TIPO_NULO, CTRL_NORMAL };
    if (node && node->left) {
        TipoRetorno evaluar = ejecutar(node->left, e);
        res.valor   = evaluar.valor;
        res.tipo    = evaluar.tipo;
    }
    res.control = CTRL_RETURN;
    return res;
}

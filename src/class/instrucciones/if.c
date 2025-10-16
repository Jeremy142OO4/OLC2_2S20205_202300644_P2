#include <stdio.h>
#include "if.h"
#include <string.h>

TipoRetorno ejecutarIf(struct ASTNode* node, struct entorno* e) {
    if (!node) return (TipoRetorno){ NULL, TIPO_NULO, CTRL_NORMAL };

    TipoRetorno cond = ejecutar(node->left, e);

    struct ASTNode* par   = node->right;
    struct ASTNode* thenb = par ? par->left  : NULL;
    struct ASTNode* elseb = par ? par->right : NULL;

    if (*(int*)cond.valor) {
        if (thenb) {
            struct entorno* entorno_if = nuevo_entorno(e);
            return ejecutar(thenb, entorno_if);   // ← PROPAGA control/valor del then
        }
        return (TipoRetorno){ NULL, TIPO_NULO, CTRL_NORMAL };
    } else {
        if (elseb) {
            if (elseb->kind && strcmp(elseb->kind, "if") == 0) {
                return ejecutarIf(elseb, e);      // ← encadena else-if
            } else {
                struct entorno* entorno_else = nuevo_entorno(e);
                return ejecutar(elseb, entorno_else); // ← PROPAGA control/valor del else
            }
        }
        return (TipoRetorno){ NULL, TIPO_NULO, CTRL_NORMAL };
    }
}

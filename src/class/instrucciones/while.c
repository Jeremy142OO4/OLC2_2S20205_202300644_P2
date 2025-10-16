#include <stdio.h>
#include "while.h"
#include <string.h>
#include "while.h"



void cicloWhile(struct ASTNode* node, struct entorno* entorno) {
    if (!node) return;

    TipoRetorno cond = ejecutar(node->left, entorno);

    if(*(int*)cond.valor) {
        struct ASTNode* body = node->right;
        if (body) {
            
            TipoRetorno r = ejecutar(body, entorno);  
            if (r.control == CTRL_BREAK) return;
            if (r.control == CTRL_CONTINUE) {                         
                cicloWhile(node, entorno);
                return;
            }

        }
        cicloWhile(node, entorno);
    } else {
        return;
    }
    
}


void ejecutarWhile(struct ASTNode* node, struct entorno* entorno) {
    if (!node) return;

    struct entorno* entorno_while = nuevo_entorno(entorno);
    cicloWhile(node, entorno_while);
}

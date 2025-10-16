#include <stdio.h>
#include "for.h"
#include <string.h>


void For(struct ASTNode* node, struct entorno* entorno) {
    if (!node) return;
    struct entorno* entorno_for = nuevo_entorno(entorno);
    TipoRetorno cond = ejecutar(node->left->right, entorno_for);

    if(*(int*)cond.valor) {
        struct ASTNode* instrucciones = node->right->right;
        struct ASTNode* cambio = node->right->left;
        if (instrucciones) {
            TipoRetorno r = ejecutar(instrucciones, entorno_for);        
            if (r.control == CTRL_BREAK) return;
        }
        ejecutar(cambio, entorno_for);
        For(node, entorno_for);
        
    } else {
        return;
    }
    
}

void ejecutarFor(struct ASTNode* node, struct entorno* entorno) {
    if (!node) return;


    struct entorno* entorno_for= nuevo_entorno(entorno);
    struct ASTNode* init = node->left ? node->left->left : NULL;
    ejecutar(init, entorno_for);


    For(node, entorno_for);
}
// join.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "join.h"
#include "../expresiones/ejecutar.h"
#include "../entorno/entorno.h"
#include "../entorno/symbol.h"

TipoRetorno ejecutarJoin(struct ASTNode* node, struct entorno* e) {
    TipoRetorno res = (TipoRetorno){ NULL, TIPO_NULO, CTRL_NORMAL };
    if (!node || !node->left || !node->right) {
        printf("Error: nodo inválido en join\n");
        return res;
    }

    // 1) delimitador
    TipoRetorno d = ejecutar(node->left, e);
    if (d.tipo != TIPO_CADENA) {
        printf("Error: String.join requiere delimitador string\n");
        return res;
    }
    const char* delim = (const char*)d.valor;
    size_t dlen = (delim ? strlen(delim) : 0);


    struct ASTNode* args = node->right;
    int es_solo_uno = (args && args->left && args->right == NULL);

    int usar_arreglo = 0;
    symbol* arrSym = NULL;
    if (es_solo_uno && args->left && args->left->kind && strcmp(args->left->kind, "id") == 0) {
        arrSym = getVar(e, args->left->value);
        if (arrSym && arrSym->es_arreglo == 1 && arrSym->tipo == TIPO_CADENA) {
            usar_arreglo = 1;
        }
    }

  
    size_t total = 0;
    int count = 0;

    if (usar_arreglo) {
        char** vec = (char**)arrSym->valor;
        int n = arrSym->longitud;
        for (int i = 0; i < n; i++) {
            const char* s = (vec ? vec[i] : NULL);
            if (s) total += strlen(s);
            if (i + 1 < n) total += dlen;
        }
        count = n;
    } else {
        for (struct ASTNode* it = args; it; it = it->right) {
            TipoRetorno ev = ejecutar(it->left, e);
            if (ev.tipo != TIPO_CADENA) {
                printf("Error: String.join solo admite elementos string (o un arreglo de strings)\n");
                return res;
            }
            if (ev.valor) total += strlen((char*)ev.valor);
            if (it->right) total += dlen;
            count++;
        }
    }

    // 3) Reservar memoria
    char* out = (char*)malloc(total + 1);
    if (!out) { printf("Error: OOM en join\n"); return res; }
    out[0] = '\0';

    // 4) Construir resultado
    if (usar_arreglo) {
        char** vec = (char**)arrSym->valor;
        for (int i = 0; i < count; i++) {
            const char* s = (vec ? vec[i] : NULL);
            if (s) strcat(out, s);
            if (i + 1 < count) strcat(out, delim);
        }
    } else {
        for (struct ASTNode* it = args; it; it = it->right) {
            TipoRetorno ev = ejecutar(it->left, e);
            const char* s = (const char*)ev.valor;
            if (s) strcat(out, s);
            if (it->right) strcat(out, delim);
        }
    }

    res.valor = out;
    res.tipo = TIPO_CADENA;
    return res;
}

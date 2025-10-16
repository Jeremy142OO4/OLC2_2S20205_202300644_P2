#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "declarar_vector.h"

void ejecutarDeclararVector(struct ASTNode* node, struct entorno* e) {
    if (!node || !node->value) return;

    const char* id = node->value;

    int es_literal = (node->right != NULL && node->left && !node->left->right);
    struct ASTNode* tipoN = es_literal ? node->left : (node->left ? node->left->left : NULL);

    Tipo elem = TIPO_NULO;
    if      (tipoN && strcmp(tipoN->value,"int")==0)    elem = TIPO_ENTERO;
    else if (tipoN && strcmp(tipoN->value,"float")==0)  elem = TIPO_DECIMAL;
    else if (tipoN && strcmp(tipoN->value,"string")==0) elem = TIPO_CADENA;
    else if (tipoN && strcmp(tipoN->value,"bool")==0)   elem = TIPO_BOOLEANO;
    else if (tipoN && strcmp(tipoN->value,"char")==0)   elem = TIPO_CHAR;
    else if (tipoN && strcmp(tipoN->value,"double")==0) elem = TIPO_DOUBLE;

    size_t esz = 0;
    switch (elem) {
        case TIPO_ENTERO:   esz = sizeof(int);    break;
        case TIPO_DECIMAL:  esz = sizeof(float);  break;
        case TIPO_DOUBLE:   esz = sizeof(double); break;
        case TIPO_CHAR:     esz = sizeof(char);   break;
        case TIPO_BOOLEANO: esz = sizeof(int);    break;
        case TIPO_CADENA:   esz = sizeof(char*);  break;
        default: break;
    }

    symbol* s = (symbol*)malloc(sizeof(symbol));
    s->id = strdup(id);
    s->tipo = elem;
    s->constante = 0;
    s->es_arreglo = 1;
    s->dimensiones = 1;
    s->filas = s->columnas = 0;
    s->siguiente = NULL;

    if (!es_literal) {
        struct ASTNode* tamN = node->left ? node->left->right : NULL;
        TipoRetorno tR = ejecutar(tamN, e);
        int len = (tR.tipo==TIPO_ENTERO) ? *(int*)tR.valor : 0;
        s->longitud = len;
        s->valor = (esz>0 && len>0) ? calloc((size_t)len, esz) : NULL;
    } else {
        struct ASTNode* lista = node->right;
        int n = 0; for (struct ASTNode* it=lista; it; it=it->right) n++;
        s->longitud = n;
        s->valor = (esz>0 && n>0) ? malloc(esz*(size_t)n) : NULL;

        int i=0;
        for (struct ASTNode* it=lista; it; it=it->right, ++i) {
            TipoRetorno v = ejecutar(it->left, e);
            char* base = (char*)s->valor + (size_t)i*esz;
            if (elem==TIPO_CADENA) {
                *(char**)base = (char*)v.valor;
            } else {
                memcpy(base, v.valor, esz);
            }
        }
    }

    setVar(e, s);
}
#include "vector_index.h"
#include <stdio.h>
#include <stdlib.h>

static size_t elem_size(Tipo t) {
    switch (t) {
        case TIPO_ENTERO:   return sizeof(int);
        case TIPO_DECIMAL:  return sizeof(float);
        case TIPO_CHAR:     return sizeof(char);
        case TIPO_BOOLEANO: return sizeof(int);
        case TIPO_CADENA:   return sizeof(char*);
        default:            return 0;
    }
}

TipoRetorno ejecutarVectorGet(struct ASTNode* indexNode, struct entorno* e) {
    TipoRetorno res; res.valor=NULL; res.tipo=TIPO_NULO; res.control=CTRL_NORMAL;
    if (!indexNode || !indexNode->left || !indexNode->right) return res;

    // id
    const char* nombre = indexNode->left->value;
    symbol* s = getVar(e, nombre);
    if (!s || !s->es_arreglo || s->dimensiones!=1) return res;

    // índice
    TipoRetorno trI = ejecutar(indexNode->right, e);
    int i = (trI.tipo==TIPO_ENTERO) ? *(int*)trI.valor : -1;
    if (i < 0 || i >= s->longitud) return res;

    size_t esz = elem_size(s->tipo);
    char* base = (char*)s->valor + (size_t)i*esz;

    
    res.tipo = s->tipo;
    switch (s->tipo) {
        case TIPO_ENTERO:   { int* v=malloc(sizeof(int));   *v=*(int*)base;    res.valor=v; break; }
        case TIPO_DECIMAL:  { float* v=malloc(sizeof(float));*v=*(float*)base; res.valor=v; break; }
        case TIPO_CHAR:     { char* v=malloc(sizeof(char)); *v=*(char*)base;   res.valor=v; break; }
        case TIPO_BOOLEANO: { int* v=malloc(sizeof(int));   *v=*(int*)base;    res.valor=v; break; }
        case TIPO_CADENA:   { res.valor = *(char**)base; break; } // puntero tal cual
        default: break;
    }
    return res;
}

void ejecutarVectorSet(struct ASTNode* indexNode, struct ASTNode* valueExpr, struct entorno* e) {
    if (!indexNode || !indexNode->left || !indexNode->right) return;

    const char* nombre = indexNode->left->value;
    symbol* s = getVar(e, nombre);
    if (!s || !s->es_arreglo || s->dimensiones!=1) return;

    // índice
    TipoRetorno trI = ejecutar(indexNode->right, e);
    int i = (trI.tipo==TIPO_ENTERO) ? *(int*)trI.valor : -1;
    if (i < 0 || i >= s->longitud) return;

    // valor
    TipoRetorno v = ejecutar(valueExpr, e);
    if (v.tipo != s->tipo) return; // puedes relajar esto si quieres casting implícito

    size_t esz = elem_size(s->tipo);
    char* base = (char*)s->valor + (size_t)i*esz;

    if (s->tipo==TIPO_CADENA) {
        *(char**)base = (char*)v.valor; // guarda puntero
    } else {
        memcpy(base, v.valor, esz);     // copia bytes
    }
}

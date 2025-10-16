#include <stdio.h>
#include <string.h>
#include <stdlib.h>   
#include "id.h"
#include "length.h"

TipoRetorno ejecutarLength(struct ASTNode* node, struct entorno* e) {
    TipoRetorno res;
    res.valor = NULL;
    res.tipo = TIPO_NULO;
    res.control = CTRL_NORMAL;

    symbol* s = getVar(e, node->value);
    if (!s){
        return res;
    }

    if (!s->es_arreglo) {
        printf("Error: '%s' no es un arreglo\n", node->value);
        return res;
    }
    int* val = malloc(sizeof(int));

    *val = s->longitud;
    res.valor = val;
    res.tipo = TIPO_ENTERO;

    return res;
}

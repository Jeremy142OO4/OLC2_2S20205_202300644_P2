#include "array_add.h"
#include "../entorno/symbol.h"
#include "../expresiones/ejecutar.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static size_t elem_size(Tipo t) {
    switch (t) {
        case TIPO_ENTERO:   return sizeof(int);
        case TIPO_DECIMAL:  return sizeof(float);
        case TIPO_DOUBLE:   return sizeof(double);
        case TIPO_CHAR:     return sizeof(char);
        case TIPO_BOOLEANO: return sizeof(int);
        case TIPO_CADENA:   return sizeof(char*); /* guardamos punteros a char */
        default:            return 0;
    }
}

void ejecutarArrayAdd(struct ASTNode* node, struct entorno* e) {
    if (!node || !node->left || !node->right) {
        printf("Error: array.add requiere id y un valor\n");
        return;
    }

    /* node->left es un id: lo usamos para obtener el símbolo del arreglo */
    struct symbol* arr = getVar(e, node->left->value);
    if (!arr || !arr->es_arreglo) {
        printf("Error: '%s' no es un arreglo\n", node->left->value);
        return;
    }

    /* evaluamos el valor a insertar */
    TipoRetorno v = ejecutar(node->right, e);

    /* tipos deben coincidir con el tipo de elemento del arreglo */
    if (v.tipo != arr->tipo) {
        printf("Error: array.add tipo incompatible (arr=%d, val=%d)\n", arr->tipo, v.tipo);
        return;
    }

    size_t esz = elem_size(arr->tipo);
    if (esz == 0) {
        printf("Error: tipo de arreglo no soportado en add\n");
        return;
    }

    int nuevo_len = arr->longitud + 1;
    void* nuevo = realloc(arr->valor, (size_t)nuevo_len * esz);
    if (!nuevo) {
        printf("Error: sin memoria en array.add\n");
        return;
    }

    /* escribe al final */
    char* base = (char*)nuevo + (size_t)arr->longitud * esz;

    if (arr->tipo == TIPO_CADENA) {
        /* para strings guardamos el puntero; duplica si quieres copiar contenido */
        *(char**)base = (char*)v.valor;
    } else if (arr->tipo == TIPO_ENTERO) {
        *(int*)base = *(int*)v.valor;
    } else if (arr->tipo == TIPO_DECIMAL) {
        *(float*)base = *(float*)v.valor;
    } else if (arr->tipo == TIPO_DOUBLE) {
        *(double*)base = *(double*)v.valor;
    } else if (arr->tipo == TIPO_CHAR) {
        *(char*)base = *(char*)v.valor;
    } else if (arr->tipo == TIPO_BOOLEANO) {
        *(int*)base = *(int*)v.valor ? 1 : 0;
    }

    /* actualiza símbolo */
    arr->valor = nuevo;
    arr->longitud = nuevo_len;
    /* listo: in-place */
}

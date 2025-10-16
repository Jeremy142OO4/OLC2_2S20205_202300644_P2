#include "indexOf.h"
#include "../expresiones/ejecutar.h"
#include "../entorno/entorno.h"
#include "../entorno/symbol.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

TipoRetorno ejecutarIndexOf(struct ASTNode* node, struct entorno* e) {
    TipoRetorno res = { NULL, TIPO_ENTERO, CTRL_NORMAL };
    int *salida = (int*)malloc(sizeof(int));
    if (!salida) return res;
    *salida = -1;
    res.valor = salida;

    if (!node || !node->left || !node->right) {
        printf("Error: Arrays.indexOf requiere 2 parámetros\n");
        return res;
    }

    if (!node->left->kind || strcmp(node->left->kind, "id") != 0) {
        printf("Error: Arrays.indexOf espera un identificador como primer argumento\n");
        return res;
    }

    symbol* arr = getVar(e, node->left->value);
    if (!arr || !arr->es_arreglo) {
        printf("Error: '%s' no es un arreglo\n", node->left->value);
        return res;
    }


    TipoRetorno key = ejecutar(node->right, e);

    for (int i = 0; i < arr->longitud; i++) {
        int match = 0;
        switch (arr->tipo) {
            case TIPO_ENTERO:
                if (key.tipo == TIPO_ENTERO)
                    match = (((int*)arr->valor)[i] == *(int*)key.valor);
                break;

            case TIPO_DECIMAL: 
                if (key.tipo == TIPO_DECIMAL)
                    match = (((float*)arr->valor)[i] == *(float*)key.valor);
                break;

            case TIPO_DOUBLE: 
                if (key.tipo == TIPO_DOUBLE)
                    match = (((double*)arr->valor)[i] == *(double*)key.valor);
                break;

            case TIPO_CADENA: {
                if (key.tipo == TIPO_CADENA) {
                    char* elem = ((char**)arr->valor)[i];
                    char* k    = (char*)key.valor;
                    if (elem && k) match = (strcmp(elem, k) == 0);
                }
                break;
            }

            case TIPO_CHAR:
                if (key.tipo == TIPO_CHAR)
                    match = (((char*)arr->valor)[i] == *(char*)key.valor);
                break;

            case TIPO_BOOLEANO:
                if (key.tipo == TIPO_BOOLEANO)
                    match = (((int*)arr->valor)[i] == *(int*)key.valor);
                break;

            default:

                break;
        }

        if (match) { *salida = i; break; }
    }

    return res;
}

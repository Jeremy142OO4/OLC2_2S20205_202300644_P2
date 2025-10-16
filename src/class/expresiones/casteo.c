#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "casteo.h"

TipoRetorno ejecutarCasteo(const char* tipoDestino, TipoRetorno valor) {
    TipoRetorno res;
    res.valor = NULL;
    res.tipo  = TIPO_NULO;
    res.control = CTRL_NORMAL;

    if (!tipoDestino) {
        printf("Error: tipo destino nulo en casteo\n");
        return res;
    }

    if (strcmp(tipoDestino, "int") == 0) {
        int *p = (int*)malloc(sizeof(int));
        if (!p) return res;

        if      (valor.tipo == TIPO_DECIMAL) *p = (int)(*(float*)valor.valor);
        else if (valor.tipo == TIPO_DOUBLE)  *p = (int)(*(double*)valor.valor);
        else if (valor.tipo == TIPO_CHAR)    *p = (int)(*(char*)valor.valor);
        else if (valor.tipo == TIPO_ENTERO)  *p = *(int*)valor.valor;
        else { free(p); printf("Casteo a int no soportado.\n"); return res; }

        res.valor = p;
        res.tipo  = TIPO_ENTERO;
    }
    else if (strcmp(tipoDestino, "float") == 0) {
        float *p = (float*)malloc(sizeof(float));
        if (!p) return res;

        if      (valor.tipo == TIPO_ENTERO)  *p = (float)(*(int*)valor.valor);
        else if (valor.tipo == TIPO_CHAR)    *p = (float)(*(char*)valor.valor);
        else if (valor.tipo == TIPO_DECIMAL) *p = *(float*)valor.valor;
        else if (valor.tipo == TIPO_DOUBLE)  *p = (float)(*(double*)valor.valor);  // << clave
        else { free(p); printf("Casteo a float no soportado.\n"); return res; }

        res.valor = p;
        res.tipo  = TIPO_DECIMAL;
    }
    else if (strcmp(tipoDestino, "double") == 0) {
        double *p = (double*)malloc(sizeof(double));
        if (!p) return res;

        if      (valor.tipo == TIPO_ENTERO)  *p = (double)(*(int*)valor.valor);
        else if (valor.tipo == TIPO_CHAR)    *p = (double)(*(char*)valor.valor);
        else if (valor.tipo == TIPO_DECIMAL) *p = (double)(*(float*)valor.valor);
        else if (valor.tipo == TIPO_DOUBLE)  *p = *(double*)valor.valor;
        else { free(p); printf("Casteo a double no soportado.\n"); return res; }

        res.valor = p;
        res.tipo  = TIPO_DOUBLE;
    }
    else if (strcmp(tipoDestino, "char") == 0) {
        char *p = (char*)malloc(sizeof(char));
        if (!p) return res;

        if      (valor.tipo == TIPO_ENTERO)  *p = (char)(*(int*)valor.valor);
        else if (valor.tipo == TIPO_DECIMAL) *p = (char)(*(float*)valor.valor);
        else if (valor.tipo == TIPO_DOUBLE)  *p = (char)(*(double*)valor.valor);
        else if (valor.tipo == TIPO_CHAR)    *p = *(char*)valor.valor;
        else { free(p); printf("Casteo a char no soportado.\n"); return res; }

        res.valor = p;
        res.tipo  = TIPO_CHAR;
    }
    else {
        printf("Error: tipo de casteo no soportado: %s\n", tipoDestino);
    }

    return res;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logico.h"

TipoRetorno ejecutarLogico(const char* op, TipoRetorno a, TipoRetorno b) {
    TipoRetorno res;
    res.valor = NULL;
    res.tipo = TIPO_NULO;
    res.control = CTRL_NORMAL;

    int *val = malloc(sizeof(int));
    if (!val) {
        res.valor = NULL;
        res.tipo  = TIPO_NULO;
        return res;
    }

    if (strcmp(op, "&&") == 0) {
        int izq = (a.valor ? (*(int*)a.valor != 0) : 0);
        int der = (b.valor ? (*(int*)b.valor != 0) : 0);
        *val = (izq && der);

        res.valor = val;
        res.tipo  = TIPO_BOOLEANO;
    }
    else if (strcmp(op, "||") == 0) {
        int izq = (a.valor ? (*(int*)a.valor != 0) : 0);
        int der = (b.valor ? (*(int*)b.valor != 0) : 0);
        *val = (izq || der);

        res.valor = val;
        res.tipo  = TIPO_BOOLEANO;
    }
    else if (strcmp(op, "!") == 0) {
        int *val = malloc(sizeof(int));
        int izq = (a.valor ? (*(int*)a.valor != 0) : 0);
        *val = !izq;
        res.valor = val;
        res.tipo  = TIPO_BOOLEANO;
        return res;
    }


    else {
        printf("Error: operador relacional no soportado '%s'\n", op);
        free(val);
        return res;
    }

    res.valor = val;
    res.tipo = TIPO_BOOLEANO;
    return res;
}
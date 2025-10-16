#include <stdio.h>
#include <string.h>
#include "llamada_funcion.h"
#include "declarar_parametros.h"

static int contar_parametros(struct ASTNode* parametros) {
    int n = 0;
    while (parametros) { n++; parametros = parametros->right; }
    return n;
}

TipoRetorno ejecutarLlamadaFuncion(struct ASTNode* node, struct entorno* e) {
    TipoRetorno res; res.valor=NULL; res.tipo=TIPO_NULO; res.control=CTRL_NORMAL;

    if (!node || !node->value) {
        printf("Error: llamada de función sin nombre\n");
        return res;
    }

    struct funcion* func = getFunc(e, node->value);
    if (!func) {
        printf("Error: función '%s' no definida\n", node->value);
        return res;
    }

    if (contar_parametros(node->left) != func->num_parametros) {
        printf("Error: número de parámetros incorrecto en llamada a '%s'\n", node->value);
        return res;
    }

    struct entorno* entorno_funcion = nuevo_entorno(e);

    if (func->parametros) {
        ejecutarDeclararParametros(func->parametros, entorno_funcion);
    }

    struct ASTNode* formal = func->parametros;
    struct ASTNode* actual = node->left;

    while (formal && actual) {
        struct symbol *s = getVar(entorno_funcion, formal->value);
        if (!s) {
            printf("Error: parámetro '%s' no declarado en función '%s'\n", formal->value, func->id);
            return res;
        }

        if (s->es_arreglo) {
            struct symbol *arg = NULL;

            if (actual->left && actual->left->value) {
                arg = getVar(e, actual->left->value);
            }

            if (!arg) {
                if (actual->left && actual->left->kind &&
                    strcmp(actual->left->kind, "index1") == 0) {
                    printf("Error: se recibió un elemento de arreglo para el parámetro '%s', se requiere el arreglo completo\n", formal->value);
                } else {
                    printf("Error: se esperaba un identificador de arreglo para el parámetro '%s'\n", formal->value);
                }
                return res;
            }

            if (!arg->es_arreglo) {
                printf("Error: el argumento '%s' no es un arreglo para el parámetro '%s'\n",
                       actual->left->value, formal->value);
                return res;
            }

            if (s->tipo != arg->tipo) {
                printf("Error: tipo base de arreglo incompatible en parámetro '%s'\n", formal->value);
                return res;
            }

            s->valor       = arg->valor;      /* por referencia */
            s->tipo        = arg->tipo;
            s->longitud    = arg->longitud;
            s->dimensiones = arg->dimensiones;
            s->filas       = arg->filas;
            s->columnas    = arg->columnas;
            s->es_arreglo  = 1;
        } else {
            TipoRetorno valor = ejecutar(actual->left, e);
            if (valor.tipo != s->tipo) {
                printf("Error: tipo de parámetro '%s' incorrecto en función '%s'\n", formal->value, func->id);
                return res;
            }
            s->valor = valor.valor;
            s->tipo  = valor.tipo;
        }

        formal = formal->right;
        actual = actual->right;
    }

    struct ASTNode* instrucciones = func->instrucciones;
    if (instrucciones) {
        res = ejecutar(instrucciones, entorno_funcion);
        if (res.control == CTRL_RETURN) {
            if (res.tipo != func->retorno) {
                printf("Error: tipo de retorno incorrecto en función '%s'\n", func->id);
                res.valor = NULL; res.tipo = TIPO_NULO; res.control = CTRL_NORMAL;
                return res;
            }
        }
        res.control = CTRL_NORMAL;
    }

    return res;
}
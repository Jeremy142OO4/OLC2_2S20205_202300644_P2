#include "ejecutar.h"
#include "entorno.h"
#include "symbol.h"
#include "./aritmetico.h"
#include "./literal.h"
#include "./id.h"
#include "./relacional.h"
#include "./casteo.h"
#include "./logico.h"
#include "./length.h"
#include "./llamada_funcion.h"
#include "../asm/gen_expr.h"
#include "./join.h"
#include "./parser.h"
#include "./vector_index.h"
#include "./indexOf.h"
#include "../instrucciones/imprimir.h"
#include "../instrucciones/declarar_var.h"
#include "../instrucciones/asignacion.h"
#include "../instrucciones/if.h"
#include "../instrucciones/incremento_decremento.h"
#include "../instrucciones/switch.h"
#include "../instrucciones/while.h"
#include "../instrucciones/break.h"
#include "../instrucciones/continuar.h"
#include "../instrucciones/retornar.h"
#include "../instrucciones/for.h"
#include "../instrucciones/declarar_funcion.h"
#include "../instrucciones/declarar_parametros.h"
#include "../instrucciones/declarar_vector.h"
#include "../instrucciones/array_add.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

TipoRetorno ejecutar(struct ASTNode *node, struct entorno *entorno)
{
    TipoRetorno res;
    res.valor = NULL;
    res.tipo = TIPO_NULO;
    res.control = CTRL_NORMAL;

    if (!node)
        return res;

    // printf("Ejecutando nodo: %s\n", node->kind);

    if (strcmp(node->kind, "literal") == 0)
    {
        res = ejecutarLiteral(node->value);
    }
    else if (strcmp(node->kind, "id") == 0)
    {
        res = ejecutarId(entorno, node->value);
    }
    else if (strcmp(node->kind, "binop") == 0)
    {
        TipoRetorno exp1 = ejecutar(node->left, entorno);
        TipoRetorno exp2 = ejecutar(node->right, entorno);
        if (strcmp(node->value, "+") == 0 || strcmp(node->value, "-") == 0 || strcmp(node->value, "*") == 0 || strcmp(node->value, "/") == 0 || strcmp(node->value, "%") == 0 || strcmp(node->value, "&") == 0 || strcmp(node->value, "|") == 0 || strcmp(node->value, "^") == 0 || strcmp(node->value, ">>") == 0 || strcmp(node->value, "<<") == 0)
        {
            res = ejecutarAritmetico(node->value, exp1, exp2);
        }
        else if (strcmp(node->value, "==") == 0 || strcmp(node->value, "!=") == 0 || strcmp(node->value, "<") == 0 || strcmp(node->value, "<=") == 0 || strcmp(node->value, ">") == 0 || strcmp(node->value, ">=") == 0)
        {
            TipoRetorno exp1 = ejecutar(node->left, entorno);
            TipoRetorno exp2 = ejecutar(node->right, entorno);
            res = ejecutarRelacional(node->value, exp1, exp2);
        }
        else if (strcmp(node->value, "&&") == 0 || strcmp(node->value, "||") == 0)
        {
            res = ejecutarLogico(node->value, exp1, exp2);
        }

        else
        {
            printf("Error: operación binaria no soportada: %s\n", node->value);
            return res;
        }
    }
    else if (strcmp(node->kind, "unop") == 0)
    {
        TipoRetorno val = ejecutar(node->left, entorno);
        if (strcmp(node->value, "-") == 0 && val.tipo == TIPO_ENTERO)
        {
            int *p = malloc(sizeof(int));
            *p = -(*(int *)val.valor);
            res.tipo = TIPO_ENTERO;
            res.valor = p;
            return res;
        }
        else if (strcmp(node->value, "-") == 0 && val.tipo == TIPO_DOUBLE)
        {
            double *p = malloc(sizeof(double));
            *p = -(*(double *)val.valor);
            res.tipo = TIPO_DOUBLE;
            res.valor = p;
            return res;
        }
        else if (strcmp(node->value, "-") == 0 && val.tipo == TIPO_DECIMAL)
        {
            float *p = malloc(sizeof(float));
            *p = -(*(float *)val.valor);
            res.tipo = TIPO_DECIMAL;
            res.valor = p;
            return res;
        }
        TipoRetorno exp1 = ejecutar(node->left, entorno);
        if (strcmp(node->value, "!") == 0)
        {
            TipoRetorno nulo = {NULL, TIPO_NULO};
            res = ejecutarLogico(node->value, exp1, nulo);
        }
        else
        {
            printf("Error: operación unaria no soportada: %s\n", node->value);
            return res;
        }
    }
    else if (strcmp(node->kind, "print") == 0)
    {
        TipoRetorno valor = ejecutar(node->left, entorno);
        ejecutarImprimir(valor);
        gen_imprimir_expr(node->left);
    }
    else if (strcmp(node->kind, "var") == 0)
    {
        ejecutarDeclarar(node, entorno, 0);
    }
    else if (strcmp(node->kind, "var_const") == 0)
    {
        ejecutarDeclarar(node, entorno, 1);
    }
    else if (strcmp(node->kind, "assign") == 0)
    {
        res = ejecutarAsignacion(node, entorno);
    }
    else if (strcmp(node->kind, "cast") == 0)
    {
        TipoRetorno valor = ejecutar(node->left, entorno);
        const char *tipoDestino = node->right ? node->right->value : NULL;
        res = ejecutarCasteo(tipoDestino, valor);
    }
    else if (strcmp(node->kind, "if") == 0)
    {

        res = ejecutarIf(node, entorno);
    }
    else if (strcmp(node->kind, "incremento_decremento") == 0)
    {
        res = ejecutarIncremento_Decremento(node, entorno);
    }
    else if (strcmp(node->kind, "switch") == 0)
    {
        ejecutarSwitch(node, entorno);
    }
    else if (strcmp(node->kind, "while") == 0)
    {
        ejecutarWhile(node, entorno);
    }
    else if (strcmp(node->kind, "for") == 0)
    {
        ejecutarFor(node, entorno);
    }
    else if (strcmp(node->kind, "break") == 0)
    {
        res = ejecutarBreak();
    }
    else if (strcmp(node->kind, "continue") == 0)
    {
        res = ejecutarContinuar();
    }
    else if (strcmp(node->kind, "return") == 0)
    {
        res = ejecutarRetornar(node, entorno);
    }
    else if (strcmp(node->kind, "funcion_decl") == 0)
    {

        res = ejecutarDeclararFuncion(node, entorno);
    }
    else if (strcmp(node->kind, "parametro") == 0)
    {

        ejecutarDeclararParametros(node, entorno);
    }
    else if (strcmp(node->kind, "llamada_funcion") == 0)
    {
        res = ejecutarLlamadaFuncion(node, entorno);
    }
    else if (strcmp(node->kind, "parser") == 0)
    {
        res = ejecutarParser(node, entorno);
    }
    else if (strcmp(node->kind, "vector_decl") == 0)
    {
        ejecutarDeclararVector(node, entorno);
    }
    else if (strcmp(node->kind, "vector_decl_init") == 0)
    {
        ejecutarDeclararVector(node, entorno);
    }
    else if (strcmp(node->kind, "index1") == 0)
    {

        res = ejecutarVectorGet(node, entorno);
    }
    else if (strcmp(node->kind, "array_length") == 0)
    {

        res = ejecutarLength(node, entorno);
    }
    else if (strcmp(node->kind, "join") == 0)
    {

        res = ejecutarJoin(node, entorno);
    }
    else if (strcmp(node->kind, "array_indexof") == 0)
    {
        res = ejecutarIndexOf(node, entorno);
    }
    else if (strcmp(node->kind, "array_add") == 0)
    {
        ejecutarArrayAdd(node, entorno);
    }
    else if (strcmp(node->kind, "link") == 0)
    {
        TipoRetorno r1 = ejecutar(node->left, entorno);

        if (r1.control != CTRL_NORMAL)
            return r1;

        TipoRetorno r2 = ejecutar(node->right, entorno);
        return r2;
    }

    return res;
}
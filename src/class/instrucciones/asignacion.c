#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../utils/errores.h"

#include "asignacion.h"
#include "../expresiones/ejecutar.h"
#include "../expresiones/vector_index.h"

TipoRetorno ejecutarAsignacion(struct ASTNode *node, struct entorno *e)
{
    TipoRetorno res = (TipoRetorno){NULL, TIPO_NULO, CTRL_NORMAL};
    if (!node || !node->left || !node->left->value)
    {
        printf("Error: asignación con nodo inválido\n");
        errores_add("Error: asignación con nodo inválido\n");
        return res;
    }

    struct symbol *s = getVar(e, node->left->value);
    if (!s)
    {
        char buffer[256]; 
        snprintf(buffer, sizeof(buffer),"Error: variable '%s' no definida para asignación\n", node->left->value);
        errores_add(buffer);
        return res;
    }
    if (s->constante == 1)
    {   
        char buffer[256]; 
        snprintf(buffer, sizeof(buffer),"Error: no se puede asignar a constante '%s'\n", s->id);
        errores_add(buffer);
        return res;
    }

    TipoRetorno val = ejecutar(node->right, e);

    if (s->tipo != val.tipo && strcmp(node->value, "=") == 0)
    {
        char buffer[256]; 
        snprintf(buffer, sizeof(buffer),"Error: asignación de tipo incompatible a la variable '%s'\n", s->id);
        errores_add(buffer);
        return res;
    }

    if (strcmp(node->value, "=") == 0)
    {
        if (node->left && strcmp(node->left->kind, "index1") == 0)
        {
            ejecutarVectorSet(node->left, node->right, e);
            return (TipoRetorno){NULL, TIPO_NULO, CTRL_NORMAL};
        }
        s->valor = val.valor;
        s->tipo = val.tipo;
        return val;
    }
    else if (strcmp(node->value, "+=") == 0)
    {
        if (s->tipo == TIPO_ENTERO && val.tipo == TIPO_ENTERO)
        {
            int *salida = malloc(sizeof(int));
            *salida = (*(int *)s->valor) + (*(int *)val.valor);
            s->valor = salida; res.valor = salida; res.tipo = TIPO_ENTERO;
        }
        else if (s->tipo == TIPO_DECIMAL && val.tipo == TIPO_DECIMAL)
        {
            float *salida = malloc(sizeof(float));
            *salida = (*(float *)s->valor) + (*(float *)val.valor);
            s->valor = salida; res.valor = salida; res.tipo = TIPO_DECIMAL;
        }
        else if (s->tipo == TIPO_DOUBLE && val.tipo == TIPO_DOUBLE)
        {
            double *salida = malloc(sizeof(double));
            *salida = (*(double *)s->valor) + (*(double *)val.valor);
            s->valor = salida; res.valor = salida; res.tipo = TIPO_DOUBLE;
        }
        else if (s->tipo == TIPO_CADENA && val.tipo == TIPO_CADENA)
        {
            size_t n1 = strlen((char *)s->valor), n2 = strlen((char *)val.valor);
            char *salida = malloc(n1 + n2 + 1);
            strcpy(salida, (char *)s->valor); strcat(salida, (char *)val.valor);
            s->valor = salida; res.valor = salida; res.tipo = TIPO_CADENA;
        }
        else { printf("Error: Operación '+=' no soportada para estos tipos\n"); }
    }
    else if (strcmp(node->value, "-=") == 0)
    {
        if (s->tipo == TIPO_ENTERO && val.tipo == TIPO_ENTERO)
        {
            int *salida = malloc(sizeof(int));
            *salida = (*(int *)s->valor) - (*(int *)val.valor);
            s->valor = salida; res.valor = salida; res.tipo = TIPO_ENTERO;
        }
        else if (s->tipo == TIPO_DECIMAL && val.tipo == TIPO_DECIMAL)
        {
            float *salida = malloc(sizeof(float));
            *salida = (*(float *)s->valor) - (*(float *)val.valor);
            s->valor = salida; res.valor = salida; res.tipo = TIPO_DECIMAL;
        }
        else if (s->tipo == TIPO_DOUBLE && val.tipo == TIPO_DOUBLE)
        {
            double *salida = malloc(sizeof(double));
            *salida = (*(double *)s->valor) - (*(double *)val.valor);
            s->valor = salida; res.valor = salida; res.tipo = TIPO_DOUBLE;
        }
        else { printf("Error: Operación '-=' no soportada para estos tipos\n"); }
    }
    else if (strcmp(node->value, "*=") == 0)
    {
        if (s->tipo == TIPO_ENTERO && val.tipo == TIPO_ENTERO)
        {
            int *salida = malloc(sizeof(int));
            *salida = (*(int *)s->valor) * (*(int *)val.valor);
            s->valor = salida; res.valor = salida; res.tipo = TIPO_ENTERO;
        }
        else if (s->tipo == TIPO_DECIMAL && val.tipo == TIPO_DECIMAL)
        {
            float *salida = malloc(sizeof(float));
            *salida = (*(float *)s->valor) * (*(float *)val.valor);
            s->valor = salida; res.valor = salida; res.tipo = TIPO_DECIMAL;
        }
        else if (s->tipo == TIPO_DOUBLE && val.tipo == TIPO_DOUBLE)
        {
            double *salida = malloc(sizeof(double));
            *salida = (*(double *)s->valor) * (*(double *)val.valor);
            s->valor = salida; res.valor = salida; res.tipo = TIPO_DOUBLE;
        }
        else { printf("Error: Operación '*=' no soportada para estos tipos\n"); }
    }
    else if (strcmp(node->value, "/=") == 0)
    {
        if (s->tipo == TIPO_ENTERO && val.tipo == TIPO_ENTERO)
        {
            int derecha = *(int *)val.valor; if (derecha == 0){ printf("Error: División por cero\n"); return res; }
            int *salida = malloc(sizeof(int));
            *salida = (*(int *)s->valor) / derecha;
            s->valor = salida; res.valor = salida; res.tipo = TIPO_ENTERO;
        }
        else if (s->tipo == TIPO_DECIMAL && val.tipo == TIPO_DECIMAL)
        {
            float derecha = *(float *)val.valor; if (derecha == 0.0f){ printf("Error: División por cero\n"); return res; }
            float *salida = malloc(sizeof(float));
            *salida = (*(float *)s->valor) / derecha;
            s->valor = salida; res.valor = salida; res.tipo = TIPO_DECIMAL;
        }
        else if (s->tipo == TIPO_DOUBLE && val.tipo == TIPO_DOUBLE)
        {
            double derecha = *(double *)val.valor; if (derecha == 0.0){ printf("Error: División por cero\n"); return res; }
            double *salida = malloc(sizeof(double));
            *salida = (*(double *)s->valor) / derecha;
            s->valor = salida; res.valor = salida; res.tipo = TIPO_DOUBLE;
        }
        else { printf("Error: Operación '/=' no soportada para estos tipos\n"); }
    }
    else if (strcmp(node->value, "%=") == 0)
    {
        if (s->tipo == TIPO_ENTERO && val.tipo == TIPO_ENTERO)
        {
            int derecha = *(int *)val.valor; if (derecha == 0){ printf("Error: División por cero\n"); return res; }
            int *salida = malloc(sizeof(int));
            *salida = (*(int *)s->valor) % derecha;
            s->valor = salida; res.valor = salida; res.tipo = TIPO_ENTERO;
        }
        else if (s->tipo == TIPO_DECIMAL && val.tipo == TIPO_DECIMAL)
        {
            float derecha = *(float *)val.valor; if (derecha == 0.0f){ printf("Error: División por cero\n"); return res; }
            float *salida = malloc(sizeof(float));
            *salida = fmodf(*(float *)s->valor, derecha);
            s->valor = salida; res.valor = salida; res.tipo = TIPO_DECIMAL;
        }
        else if (s->tipo == TIPO_DOUBLE && val.tipo == TIPO_DOUBLE)
        {
            double derecha = *(double *)val.valor; if (derecha == 0.0){ printf("Error: División por cero\n"); return res; }
            double *salida = malloc(sizeof(double));
            *salida = fmod(*(double *)s->valor, derecha);
            s->valor = salida; res.valor = salida; res.tipo = TIPO_DOUBLE;
        }
        else { printf("Error: Operación '%%=' no soportada para estos tipos\n"); }
    }
    else if (strcmp(node->value, "&=") == 0)
    {
        if (s->tipo == TIPO_ENTERO && val.tipo == TIPO_ENTERO)
        {
            int *salida = malloc(sizeof(int));
            *salida = (*(int *)s->valor) & (*(int *)val.valor);
            s->valor = salida; res.valor = salida; res.tipo = TIPO_ENTERO;
        }
        else if (s->tipo == TIPO_BOOLEANO && val.tipo == TIPO_BOOLEANO)
        {
            int *salida = malloc(sizeof(int));
            int iz = (s->valor ? (*(int *)s->valor != 0) : 0);
            int de = (val.valor ? (*(int *)val.valor != 0) : 0);
            *salida = iz & de;
            s->valor = salida; res.valor = salida; res.tipo = TIPO_BOOLEANO;
        }
        else { printf("Error: Operación '&=' no soportada para estos tipos\n"); }
    }

    else if (strcmp(node->value, "|=") == 0)
    {
        if (s->tipo == TIPO_ENTERO && val.tipo == TIPO_ENTERO)
        {
            int *salida = malloc(sizeof(int));
            *salida = (*(int *)s->valor) | (*(int *)val.valor);
            s->valor = salida; res.valor = salida; res.tipo = TIPO_ENTERO;
        }
        else if (s->tipo == TIPO_BOOLEANO && val.tipo == TIPO_BOOLEANO)
        {
            int *salida = malloc(sizeof(int));
            int iz = (s->valor ? (*(int *)s->valor != 0) : 0);
            int de = (val.valor ? (*(int *)val.valor != 0) : 0);
            *salida = iz | de;
            s->valor = salida; res.valor = salida; res.tipo = TIPO_BOOLEANO;
        }
        else { printf("Error: Operación '|=' no soportada para estos tipos\n"); }
    }
    else if (strcmp(node->value, "^=") == 0)
    {
        if (s->tipo == TIPO_ENTERO && val.tipo == TIPO_ENTERO)
        {
            int *salida = malloc(sizeof(int));
            *salida = (*(int *)s->valor) ^ (*(int *)val.valor);
            s->valor = salida; res.valor = salida; res.tipo = TIPO_ENTERO;
        }
        else if (s->tipo == TIPO_BOOLEANO && val.tipo == TIPO_BOOLEANO)
        {
            int *salida = malloc(sizeof(int));
            int iz = (s->valor ? (*(int *)s->valor != 0) : 0);
            int de = (val.valor ? (*(int *)val.valor != 0) : 0);
            *salida = iz ^ de;
            s->valor = salida; res.valor = salida; res.tipo = TIPO_BOOLEANO;
        }
        else { printf("Error: Operación '^=' no soportada para estos tipos\n"); }
    }
    else if (strcmp(node->value, ">>=") == 0)
    {
        if (s->tipo == TIPO_ENTERO && val.tipo == TIPO_ENTERO)
        {
            int *salida = malloc(sizeof(int));
            *salida = (*(int *)s->valor) >> (*(int *)val.valor);
            s->valor = salida; res.valor = salida; res.tipo = TIPO_ENTERO;
        }
        else { printf("Error: Operación '>>=' solo soportada para enteros\n"); }
    }
    else if (strcmp(node->value, "<<=") == 0)
    {
        if (s->tipo == TIPO_ENTERO && val.tipo == TIPO_ENTERO)
        {
            int *salida = malloc(sizeof(int));
            *salida = (*(int *)s->valor) << (*(int *)val.valor);
            s->valor = salida; res.valor = salida; res.tipo = TIPO_ENTERO;
        }
        else { printf("Error: Operación '<<=' solo soportada para enteros\n"); }
    }

    else
    {
        printf("Error: operador de asignación no soportado '%s'\n", node->value);
    }

    return res;
}
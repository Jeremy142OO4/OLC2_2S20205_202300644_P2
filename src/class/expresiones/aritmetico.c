#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "aritmetico.h"
#include <math.h>

static const char *convertir_cadena(TipoRetorno v, char *buffer, size_t tam)
{
    if (v.tipo == TIPO_CADENA)
    {
        return (char *)v.valor;
    }
    else if (v.tipo == TIPO_ENTERO)
    {
        snprintf(buffer, tam, "%d", *(int *)v.valor);
        return buffer;
    }
    else if (v.tipo == TIPO_CHAR)
    {
        snprintf(buffer, tam, "%c", *(char *)v.valor);
        return buffer;
    }
    else if (v.tipo == TIPO_DOUBLE)
    {
        snprintf(buffer, tam, "%.5f", *(double *)v.valor); // 6 decimales fijos
        return buffer;
    }
    else if (v.tipo == TIPO_BOOLEANO)
    {
        snprintf(buffer, tam, "%s", (*(int *)v.valor) ? "true" : "false");
        return buffer;
    }
    else
    {
        snprintf(buffer, tam, "%.2f", *(float *)v.valor);
        return buffer;
    }
}

TipoRetorno ejecutarAritmetico(const char *op, TipoRetorno a, TipoRetorno b)
{
    TipoRetorno res;
    res.valor = NULL;
    res.tipo = TIPO_NULO;
    res.control = CTRL_NORMAL;

    if (strcmp(op, "+") == 0)
    {
        if (a.tipo == TIPO_ENTERO && b.tipo == TIPO_ENTERO) {
            int *val = malloc(sizeof(int));
            *val = *(int *)a.valor + *(int *)b.valor;
            res.tipo = TIPO_ENTERO; res.valor = val;
        } else if (a.tipo == TIPO_CADENA || b.tipo == TIPO_CADENA) {
            char buffer1[64], buffer2[64];
            const char *str1 = convertir_cadena(a, buffer1, sizeof(buffer1));
            const char *str2 = convertir_cadena(b, buffer2, sizeof(buffer2));
            char *concat = malloc(strlen(str1) + strlen(str2) + 1);
            strcpy(concat, str1); strcat(concat, str2);
            res.tipo = TIPO_CADENA; res.valor = concat;
        } else if (a.tipo == TIPO_DOUBLE || b.tipo == TIPO_DOUBLE) {
            double num1 = (a.tipo == TIPO_ENTERO) ? *(int *)a.valor : (a.tipo == TIPO_DECIMAL) ? *(float *)a.valor : *(double *)a.valor;
            double num2 = (b.tipo == TIPO_ENTERO) ? *(int *)b.valor : (b.tipo == TIPO_DECIMAL) ? *(float *)b.valor : *(double *)b.valor;
            double *val = malloc(sizeof(double)); *val = num1 + num2;
            res.tipo = TIPO_DOUBLE; res.valor = val;
        } else {
            float num1 = (a.tipo == TIPO_ENTERO) ? *(int *)a.valor : *(float *)a.valor;
            float num2 = (b.tipo == TIPO_ENTERO) ? *(int *)b.valor : *(float *)b.valor;
            float *val = malloc(sizeof(float)); *val = num1 + num2;
            res.tipo = TIPO_DECIMAL; res.valor = val;
        }
    }
    else if (strcmp(op, "-") == 0)
    {
        if (a.tipo == TIPO_ENTERO && b.tipo == TIPO_ENTERO) {
            int *val = malloc(sizeof(int));
            *val = *(int *)a.valor - *(int *)b.valor;
            res.tipo = TIPO_ENTERO; res.valor = val;
        } else if (a.tipo == TIPO_DOUBLE || b.tipo == TIPO_DOUBLE) {
            double num1 = (a.tipo == TIPO_ENTERO) ? *(int *)a.valor : (a.tipo == TIPO_DECIMAL) ? *(float *)a.valor : *(double *)a.valor;
            double num2 = (b.tipo == TIPO_ENTERO) ? *(int *)b.valor : (b.tipo == TIPO_DECIMAL) ? *(float *)b.valor : *(double *)b.valor;
            double *val = malloc(sizeof(double)); *val = num1 - num2;
            res.tipo = TIPO_DOUBLE; res.valor = val;
        } else {
            float num1 = (a.tipo == TIPO_ENTERO) ? *(int *)a.valor : *(float *)a.valor;
            float num2 = (b.tipo == TIPO_ENTERO) ? *(int *)b.valor : *(float *)b.valor;
            float *val = malloc(sizeof(float)); *val = num1 - num2;
            res.tipo = TIPO_DECIMAL; res.valor = val;
        }
    }
    else if (strcmp(op, "*") == 0)
    {
        if (a.tipo == TIPO_ENTERO && b.tipo == TIPO_ENTERO) {
            int *val = malloc(sizeof(int));
            *val = *(int *)a.valor * *(int *)b.valor;
            res.tipo = TIPO_ENTERO; res.valor = val;
        } else if (a.tipo == TIPO_DOUBLE || b.tipo == TIPO_DOUBLE) {
            double num1 = (a.tipo == TIPO_ENTERO) ? *(int *)a.valor : (a.tipo == TIPO_DECIMAL) ? *(float *)a.valor : *(double *)a.valor;
            double num2 = (b.tipo == TIPO_ENTERO) ? *(int *)b.valor : (b.tipo == TIPO_DECIMAL) ? *(float *)b.valor : *(double *)b.valor;
            double *val = malloc(sizeof(double)); *val = num1 * num2;
            res.tipo = TIPO_DOUBLE; res.valor = val;
        } else {
            float num1 = (a.tipo == TIPO_ENTERO) ? *(int *)a.valor : *(float *)a.valor;
            float num2 = (b.tipo == TIPO_ENTERO) ? *(int *)b.valor : *(float *)b.valor;
            float *val = malloc(sizeof(float)); *val = num1 * num2;
            res.tipo = TIPO_DECIMAL; res.valor = val;
        }
    }
    else if (strcmp(op, "/") == 0)
    {
        if (a.tipo == TIPO_ENTERO && b.tipo == TIPO_ENTERO) {
            int divisor = *(int *)b.valor; if (divisor == 0){ printf("Error: División por cero\n"); return res; }
            int *val = malloc(sizeof(int)); *val = *(int *)a.valor / divisor;
            res.tipo = TIPO_ENTERO; res.valor = val;
        } else if (a.tipo == TIPO_DOUBLE || b.tipo == TIPO_DOUBLE) {
            double num1 = (a.tipo == TIPO_ENTERO) ? *(int *)a.valor : (a.tipo == TIPO_DECIMAL) ? *(float *)a.valor : *(double *)a.valor;
            double num2 = (b.tipo == TIPO_ENTERO) ? *(int *)b.valor : (b.tipo == TIPO_DECIMAL) ? *(float *)b.valor : *(double *)b.valor;
            if (num2 == 0.0){ printf("Error: División por cero\n"); return res; }
            double *val = malloc(sizeof(double)); *val = num1 / num2;
            res.tipo = TIPO_DOUBLE; res.valor = val;
        } else {
            float num1 = (a.tipo == TIPO_ENTERO) ? *(int *)a.valor : *(float *)a.valor;
            float num2 = (b.tipo == TIPO_ENTERO) ? *(int *)b.valor : *(float *)b.valor;
            if (num2 == 0.0f){ printf("Error: División por cero\n"); return res; }
            float *val = malloc(sizeof(float)); *val = num1 / num2;
            res.tipo = TIPO_DECIMAL; res.valor = val;
        }
    }
    else if (strcmp(op, "%") == 0)
    {
        if (a.tipo == TIPO_ENTERO && b.tipo == TIPO_ENTERO) {
            int divisor = *(int *)b.valor; if (divisor == 0){ printf("Error: Módulo por cero\n"); return res; }
            int *val = malloc(sizeof(int)); *val = *(int *)a.valor % divisor;
            res.tipo = TIPO_ENTERO; res.valor = val;
        } else if (a.tipo == TIPO_DOUBLE || b.tipo == TIPO_DOUBLE) {
            double num1 = (a.tipo == TIPO_ENTERO) ? *(int *)a.valor : (a.tipo == TIPO_DECIMAL) ? *(float *)a.valor : *(double *)a.valor;
            double num2 = (b.tipo == TIPO_ENTERO) ? *(int *)b.valor : (b.tipo == TIPO_DECIMAL) ? *(float *)b.valor : *(double *)b.valor;
            if (num2 == 0.0){ printf("Error: Módulo por cero\n"); return res; }
            double *val = malloc(sizeof(double)); *val = fmod(num1, num2);
            res.tipo = TIPO_DOUBLE; res.valor = val;
        } else {
            float num1 = (a.tipo == TIPO_ENTERO) ? *(int *)a.valor : *(float *)a.valor;
            float num2 = (b.tipo == TIPO_ENTERO) ? *(int *)b.valor : *(float *)b.valor;
            if (num2 == 0.0f){ printf("Error: Módulo por cero\n"); return res; }
            float *val = malloc(sizeof(float)); *val = fmodf(num1, num2);
            res.tipo = TIPO_DECIMAL; res.valor = val;
        }
    }
    /* ===== BITWISE (como en asignación) ===== */
    else if (strcmp(op, "&") == 0)
    {
        if (a.tipo == TIPO_ENTERO && b.tipo == TIPO_ENTERO) {
            int *val = malloc(sizeof(int));
            *val = (*(int*)a.valor) & (*(int*)b.valor);
            res.tipo = TIPO_ENTERO;
            res.valor = val;
        } else if (a.tipo == TIPO_BOOLEANO && b.tipo == TIPO_BOOLEANO) {
            int *val = malloc(sizeof(int));
            int iz = a.valor ? (*(int*)a.valor != 0) : 0;
            int de = b.valor ? (*(int*)b.valor != 0) : 0;
            *val = iz & de;
            res.tipo = TIPO_BOOLEANO; res.valor = val;
        } else {
            printf("Error: Operación '&' no soportada para estos tipos\n");
        }
    }
    else if (strcmp(op, "|") == 0)
    {
        if (a.tipo == TIPO_ENTERO && b.tipo == TIPO_ENTERO) {
            int *val = malloc(sizeof(int));
            *val = (*(int*)a.valor) | (*(int*)b.valor);
            res.tipo = TIPO_ENTERO; res.valor = val;
        } else if (a.tipo == TIPO_BOOLEANO && b.tipo == TIPO_BOOLEANO) {
            int *val = malloc(sizeof(int));
            int iz = a.valor ? (*(int*)a.valor != 0) : 0;
            int de = b.valor ? (*(int*)b.valor != 0) : 0;
            *val = iz | de;
            res.tipo = TIPO_BOOLEANO; res.valor = val;
        } else {
            printf("Error: Operación '|' no soportada para estos tipos\n");
        }
    }
    else if (strcmp(op, "^") == 0) 
    {    
        
        if (a.tipo == TIPO_ENTERO && b.tipo == TIPO_ENTERO) {
            int *val = malloc(sizeof(int));
            *val = (*(int*)a.valor) ^ (*(int*)b.valor);
            res.tipo = TIPO_ENTERO; 
            res.valor = val;
            
        } else if (a.tipo == TIPO_BOOLEANO && b.tipo == TIPO_BOOLEANO) {
            int *val = malloc(sizeof(int));
            int iz = a.valor ? (*(int*)a.valor != 0) : 0;
            int de = b.valor ? (*(int*)b.valor != 0) : 0;
            *val = iz ^ de;
            res.tipo = TIPO_BOOLEANO; res.valor = val;
        } else {
            printf("Error: Operación '^' no soportada para estos tipos\n");
        }
    }
    else if (strcmp(op, ">>") == 0)
    {
        if (a.tipo == TIPO_ENTERO && b.tipo == TIPO_ENTERO) {
            int *val = malloc(sizeof(int));
            *val = (*(int*)a.valor) >> (*(int*)b.valor);
            res.tipo = TIPO_ENTERO; res.valor = val;
        } else {
            printf("Error: Operación '>>' solo soportada para enteros\n");
        }
    }
    else if (strcmp(op, "<<") == 0)
    {
        if (a.tipo == TIPO_ENTERO && b.tipo == TIPO_ENTERO) {
            int *val = malloc(sizeof(int));
            *val = (*(int*)a.valor) << (*(int*)b.valor);
            res.tipo = TIPO_ENTERO; res.valor = val;
        } else {
            printf("Error: Operación '<<' solo soportada para enteros\n");
        }
    }
    /* ===== FIN BITWISE ===== */
    else
    {
        printf("Error: Operador no reconocido '%s'\n", op);
    }

    return res;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  
#include "literal.h"
#include <math.h> 

TipoRetorno ejecutarLiteral(const char* value) {
    TipoRetorno res;
    res.valor = NULL;
    res.tipo = TIPO_NULO;
    res.control = CTRL_NORMAL;

    if (isdigit((unsigned char)value[0])) {
        if (strchr(value, '.') != NULL) {
            size_t len = strlen(value);
            if (len > 0 && (value[len - 1] == 'f' || value[len - 1] == 'F')) {
                char tmp[64];
                strncpy(tmp, value, len - 1);
                tmp[len - 1] = '\0';
                float *val = malloc(sizeof(float));
                if (!val) { fprintf(stderr, "OOM\n"); return res; }
                *val = strtof(tmp, NULL);
                *val = roundf(*val * 100.0f) / 100.0f;
                res.valor = val;
                res.tipo = TIPO_DECIMAL;
            } else {
                double *val = malloc(sizeof(double));
                if (!val) { fprintf(stderr, "OOM\n"); return res; }
                *val = strtod(value, NULL);
                res.valor = val;
                res.tipo = TIPO_DOUBLE;
            }
        } else {
            int *val = malloc(sizeof(int));
            if (!val) { fprintf(stderr, "OOM\n"); return res; }
            *val = atoi(value);
            res.valor = val;
            res.tipo = TIPO_ENTERO;
        }
    }
    else if (value[0] == '"' && value[strlen(value) - 1] == '"') {
        size_t n = strlen(value);
        size_t end = n - 1;
        char *out = (char*)malloc(n - 1);
        if (!out) { fprintf(stderr, "OOM\n"); return res; }
        size_t i = 1, j = 0;
        while (i < end) {
            char c = value[i++];
            if (c != '\\') { out[j++] = c; continue; }
            if (i >= end) { out[j++] = '\\'; break; }
            char e = value[i++];
            switch (e) {
                case 'n':  out[j++] = '\n'; break;
                case 'r':  out[j++] = '\r'; break;
                case 't':  out[j++] = '\t'; break;
                case 'b':  out[j++] = '\b'; break;
                case 'f':  out[j++] = '\f'; break;
                case '\\': out[j++] = '\\'; break;
                case '\"': out[j++] = '\"'; break;
                case '\'': out[j++] = '\''; break;
                default:   out[j++] = '\\'; out[j++] = e; break;
            }
        }
        out[j] = '\0';
        res.valor = out;
        res.tipo  = TIPO_CADENA;
    }
    else if (value[0] == '\'' && value[strlen(value) - 1] == '\'') {
        size_t n = strlen(value);
        char *val = malloc(sizeof(char));
        if (!val) { fprintf(stderr, "OOM\n"); return res; }
        if (n == 3) {
            *val = value[1];
        } else if (n == 4 && value[1] == '\\') {
            char e = value[2];
            switch (e) {
                case 'n':  *val = '\n'; break;
                case 'r':  *val = '\r'; break;
                case 't':  *val = '\t'; break;
                case 'b':  *val = '\b'; break;
                case 'f':  *val = '\f'; break;
                case '\\': *val = '\\'; break;
                case '\'': *val = '\''; break;
                case '\"': *val = '\"'; break;
                default:   *val = e; break;
            }
        } else {
            printf("Error: literal char inválido: %s\n", value);
            free(val);
            return res;
        }
        res.valor = val;
        res.tipo = TIPO_CHAR;
    }
    else if (strcmp(value, "true") == 0 || strcmp(value, "false") == 0) {
        int *val = malloc(sizeof(int));
        if (!val) { fprintf(stderr, "OOM\n"); return res; }
        *val = (strcmp(value, "true") == 0) ? 1 : 0;
        res.valor = val;
        res.tipo = TIPO_BOOLEANO;
    }
    else {
        printf("Error: literal no soportado: %s\n", value);
    }

    return res;
}
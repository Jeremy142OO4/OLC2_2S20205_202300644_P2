#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "declarar_parametros.h"

static TipoRetorno declarar_parametros(const char* tipo) {
    TipoRetorno resultado; 
    resultado.valor = NULL; 
    resultado.tipo = TIPO_NULO ;
    resultado.control = CTRL_NORMAL;
    if (!tipo) return resultado;

    if (strcmp(tipo, "int") == 0) {
        int* p = malloc(sizeof(int));
        *p = 0;
        resultado.valor = p; 
        resultado.tipo = TIPO_ENTERO;
    }
    else if (strcmp(tipo, "float") == 0) {
        float* p = malloc(sizeof(float));
        *p = 0.0f;
        resultado.valor = p; 
        resultado.tipo = TIPO_DECIMAL;
    }
    else if (strcmp(tipo, "string") == 0) {
        resultado.valor = strdup("null");
        resultado.tipo = TIPO_CADENA;
    }
    else if (strcmp(tipo, "bool") == 0) {
        int* p = malloc(sizeof(int));
        *p = 0;
        resultado.valor = p; 
        resultado.tipo = TIPO_BOOLEANO;
    }
    else if (strcmp(tipo, "char") == 0) {
        char* p = malloc(sizeof(char));
        *p = '\0';
        resultado.valor = p; 
        resultado.tipo = TIPO_CHAR;
    }
    else if (strcmp(tipo, "double") == 0) {
        double* p = malloc(sizeof(double));
        *p = 0.0;
        resultado.valor = p; 
        resultado.tipo = TIPO_DOUBLE;
    }

    return resultado;
}

void ejecutarDeclararParametros(struct ASTNode* node, struct entorno* e) {
    if (!node || !node->value) {
        printf("Error: declaración sin identificador\n");
        return;
    }

    struct symbol* s = malloc(sizeof(struct symbol));
    s->id = strdup(node->value);
    s->siguiente = NULL;
    s->constante = 0;

    const char* tipo = (node->left && node->left->value) ? node->left->value : NULL;

    if (tipo) {
        size_t L = strlen(tipo);
        if (L >= 2 && tipo[L-2] == '[' && tipo[L-1] == ']') {
            char base[32];
            size_t bl = (L - 2) < sizeof(base)-1 ? (L - 2) : (sizeof(base)-1);
            memcpy(base, tipo, bl);
            base[bl] = '\0';
            
            if      (strcmp(base, "int")    == 0) s->tipo = TIPO_ENTERO;
            else if (strcmp(base, "float")  == 0) s->tipo = TIPO_DECIMAL;
            else if (strcmp(base, "string") == 0) s->tipo = TIPO_CADENA;
            else if (strcmp(base, "bool")   == 0) s->tipo = TIPO_BOOLEANO;
            else if (strcmp(base, "char")   == 0) s->tipo = TIPO_CHAR;
            else if (strcmp(base, "double") == 0) s->tipo = TIPO_DOUBLE;
            else                                  s->tipo = TIPO_NULO;

            s->es_arreglo  = 1;
            s->dimensiones = 1;
            s->filas = s->columnas = 0;
            s->longitud = 0;
            s->valor = NULL;
            setVar(e, s);
        } else {
            TipoRetorno def = declarar_parametros(tipo);
            s->valor = def.valor;
            s->tipo  = def.tipo;
            s->es_arreglo  = 0;
            s->dimensiones = 0;
            s->filas = s->columnas = 0;
            s->longitud = 0;
            setVar(e, s);
        }
    } else {
        printf("Error: parámetro sin tipo\n");
        free(s);
        return;
    }

    if (node->right) {
        ejecutarDeclararParametros(node->right, e);
    }
}
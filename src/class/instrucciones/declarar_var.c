#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/errores.h"
#include "declarar_var.h"

static TipoRetorno declarar(const char* t) {
    TipoRetorno r; 
    r.valor = NULL; 
    r.tipo = TIPO_NULO ;
    r.control = CTRL_NORMAL;
    if (!t) return r;

    if (strcmp(t, "int") == 0) {
        int* p = malloc(sizeof(int));
        *p = 0;
        r.valor = p; r.tipo = TIPO_ENTERO;
    }
    else if (strcmp(t, "float") == 0) {
        float* p = malloc(sizeof(float));
        *p = 0.0f;
        r.valor = p; r.tipo = TIPO_DECIMAL;
    }
    else if (strcmp(t, "double") == 0) {
        double* p = malloc(sizeof(double));
        *p = 0.0;
        r.valor = p; r.tipo = TIPO_DOUBLE;
    }
    else if (strcmp(t, "string") == 0) {
        r.valor = strdup("null");
        r.tipo = TIPO_CADENA;
    }
    else if (strcmp(t, "bool") == 0) {
        int* p = malloc(sizeof(int));
        *p = 0;
        r.valor = p; r.tipo = TIPO_BOOLEANO;
    }
    else if (strcmp(t, "char") == 0) {
        char* p = malloc(sizeof(char));
        *p = '\0';
        r.valor = p; r.tipo = TIPO_CHAR;
    }

    return r;
}


void ejecutarDeclarar(struct ASTNode* node, struct entorno* e, int es_constante) {
    if (!node || !node->value) {
        printf("Error: declaración sin identificador\n");
        return;
    }

    if (validar_var(e,node->value)){
        errores_add("Error: Ya fue declarada la variable\n");
        return;
    }

    struct symbol* s = malloc(sizeof(struct symbol));
    if (!s) { printf("Error: sin memoria para símbolo\n"); return; }

    s->id = strdup(node->value);
    s->siguiente = NULL;
    s->constante = es_constante;

    if (node->right) {
        // Declaración con inicializador
        TipoRetorno val = ejecutar(node->right, e);
        const char* t = (node->left && node->left->value) ? node->left->value : NULL;
        TipoRetorno def = declarar(t);         // Tipo de la variable (destino)
        s->tipo = def.tipo;                    // El tipo de la variable queda fijo por la declaración

        if (val.tipo == def.tipo) {
            // Tipos iguales: asignamos directo
            s->valor = val.valor;
            setVar(e, s);
            return;
        }

        // Intento de widening numérico inline: char->int->float->double
        // Mapeo de "rank" sin helpers
        int r_to =
            (def.tipo == TIPO_CHAR)    ? 0 :
            (def.tipo == TIPO_ENTERO)  ? 1 :
            (def.tipo == TIPO_DECIMAL) ? 2 :
            (def.tipo == TIPO_DOUBLE)  ? 3 : -1;

        int r_from =
            (val.tipo == TIPO_CHAR)    ? 0 :
            (val.tipo == TIPO_ENTERO)  ? 1 :
            (val.tipo == TIPO_DECIMAL) ? 2 :
            (val.tipo == TIPO_DOUBLE)  ? 3 : -1;

        if (r_from >= 0 && r_to >= 0 && r_from <= r_to) {
            // Es widening permitido
            if (def.tipo == TIPO_ENTERO) {
                // char -> int
                if (val.tipo == TIPO_CHAR) {
                    int *p = (int*)malloc(sizeof(int));
                    if (!p){ fprintf(stderr,"OOM\n"); free(s); return; }
                    *p = (int)(*(char*)val.valor);
                    s->valor = p; setVar(e, s); return;
                }
            } else if (def.tipo == TIPO_DECIMAL) {
                // char/int -> float
                float *p = (float*)malloc(sizeof(float));
                if (!p){ fprintf(stderr,"OOM\n"); free(s); return; }
                if      (val.tipo == TIPO_CHAR)   *p = (float)(*(char*)val.valor);
                else if (val.tipo == TIPO_ENTERO) *p = (float)(*(int*)val.valor);
                else /* ya sería float */         *p = *(float*)val.valor;
                s->valor = p; setVar(e, s); return;
            } else if (def.tipo == TIPO_DOUBLE) {
                // char/int/float -> double
                double *p = (double*)malloc(sizeof(double));
                if (!p){ fprintf(stderr,"OOM\n"); free(s); return; }
                if      (val.tipo == TIPO_CHAR)    *p = (double)(*(char*)val.valor);
                else if (val.tipo == TIPO_ENTERO)  *p = (double)(*(int*)val.valor);
                else if (val.tipo == TIPO_DECIMAL) *p = (double)(*(float*)val.valor);
                else /* ya sería double */         *p = *(double*)val.valor;
                s->valor = p; setVar(e, s); return;
            }
            // def.tipo == TIPO_CHAR no permite widening (sería narrowing si viene de int/float/double)
        }

        // Si llegamos aquí: no compatibles o sería narrowing
        printf("Error: Tipos no coinciden\n");
        errores_add("Error: Tipos no coinciden\n");
        free(s);
        return;
    } else {
        // Declaración sin inicializador
        if (es_constante) {
            printf("Error: una constante debe inicializarse al declararse (%s)\n", node->value);
            free(s);
            return;
        }
        const char* t = (node->left && node->left->value) ? node->left->value : NULL;
        TipoRetorno def = declarar(t);
        s->valor = def.valor;
        s->tipo  = def.tipo;
        setVar(e, s);
    }
}
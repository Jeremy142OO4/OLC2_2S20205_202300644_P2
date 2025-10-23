#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/errores.h"
#include "declarar_var.h"

/* NUEVO: solo para el compilador */
#include "../asm/asm_decl.h"
#include "../entorno/entorno.h"   // si necesitas ver entorno->anterior

static TipoRetorno declarar(const char* t) { /* ... tu código igual ... */ }

/* helper: ¿estamos en global? */
static int es_global(struct entorno* e){ return e && e->anterior == NULL; }

void ejecutarDeclarar(struct ASTNode* node, struct entorno* e, int es_constante) {
    if (!node || !node->value) { printf("Error: declaración sin identificador\n"); return; }
    if (validar_var(e,node->value)){ errores_add("Error: Ya fue declarada la variable\n"); return; }

    struct symbol* s = malloc(sizeof(struct symbol));
    if (!s) { printf("Error: sin memoria para símbolo\n"); return; }

    s->id = strdup(node->value);
    s->siguiente = NULL;
    s->constante = es_constante;
    s->es_arreglo = 0; s->dimensiones=0; s->longitud=0; s->filas=0; s->columnas=0;

    if (node->right) {
        /* declaración con inicializador */
        TipoRetorno val = ejecutar(node->right, e);
        const char* t = (node->left && node->left->value) ? node->left->value : NULL;
        TipoRetorno def = declarar(t);
        s->tipo = def.tipo;

        if (val.tipo == def.tipo) {
            s->valor = val.valor;
            setVar(e, s);

            /* ← NUEVO: si es global, emite ASM */
            if (es_global(e)) asm_decl_global(s->id, s->tipo, s->valor);
            return;
        }

        /* widening permitido (tu código igual) */
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
            if (def.tipo == TIPO_ENTERO) {
                if (val.tipo == TIPO_CHAR) {
                    int *p = (int*)malloc(sizeof(int));
                    if (!p){ fprintf(stderr,"OOM\n"); free(s); return; }
                    *p = (int)(*(char*)val.valor);
                    s->valor = p; setVar(e, s);
                } else { s->valor = val.valor; setVar(e, s); }
            } else if (def.tipo == TIPO_DECIMAL) {
                float *p = (float*)malloc(sizeof(float));
                if (!p){ fprintf(stderr,"OOM\n"); free(s); return; }
                if      (val.tipo == TIPO_CHAR)   *p = (float)(*(char*)val.valor);
                else if (val.tipo == TIPO_ENTERO) *p = (float)(*(int*)val.valor);
                else                               *p = *(float*)val.valor;
                s->valor = p; setVar(e, s);
            } else if (def.tipo == TIPO_DOUBLE) {
                double *p = (double*)malloc(sizeof(double));
                if (!p){ fprintf(stderr,"OOM\n"); free(s); return; }
                if      (val.tipo == TIPO_CHAR)    *p = (double)(*(char*)val.valor);
                else if (val.tipo == TIPO_ENTERO)  *p = (double)(*(int*)val.valor);
                else if (val.tipo == TIPO_DECIMAL) *p = (double)(*(float*)val.valor);
                else                                *p = *(double*)val.valor;
                s->valor = p; setVar(e, s);
            } else {
                /* char no widening */
                printf("Error: Tipos no coinciden\n");
                errores_add("Error: Tipos no coinciden\n");
                free(s);
                return;
            }

            /* ← NUEVO: si es global, emite ASM */
            if (es_global(e)) asm_decl_global(s->id, s->tipo, s->valor);
            return;
        }

        printf("Error: Tipos no coinciden\n");
        errores_add("Error: Tipos no coinciden\n");
        free(s);
        return;

    } else {
        /* sin inicializador */
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

        /* ← NUEVO: si es global, emite ASM */
        if (es_global(e)) asm_decl_global(s->id, s->tipo, s->valor);
    }
}

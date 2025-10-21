#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/errores.h"
#include "declarar_var.h"


#include "../utils/arm_codegen_singleton.h"
#include "../utils/codegen_arm64.h"

static void emitir_decl_arm(const char* nombre, TipoRetorno def, int tiene_init, TipoRetorno init) {
    struct CodegenARM64* H = arm_codegen_handle();
    if (!H || !nombre) return;

    switch (def.tipo) {
        case TIPO_ENTERO: {
            long long v = (tiene_init && init.valor) ? (long long)(*((int*)init.valor)) : 0;
            cg_arm64_decl_global_int(H, nombre, v, tiene_init);
        } break;
        case TIPO_BOOLEANO: {
            int v = (tiene_init && init.valor) ? (*((int*)init.valor) ? 1 : 0) : 0;
            cg_arm64_decl_global_bool(H, nombre, v, tiene_init);
        } break;
        case TIPO_CHAR: {
            int v = (tiene_init && init.valor) ? (int)(*((unsigned char*)init.valor)) : 0;
            cg_arm64_decl_global_char(H, nombre, v, tiene_init);
        } break;
        case TIPO_DECIMAL: {
            float f = (tiene_init && init.valor) ? *((float*)init.valor) : 0.0f;
            cg_arm64_decl_global_float(H, nombre, f, tiene_init);
        } break;
        case TIPO_DOUBLE: {
            double d = (tiene_init && init.valor) ? *((double*)init.valor) : 0.0;
            cg_arm64_decl_global_double(H, nombre, d, tiene_init);
        } break;
        case TIPO_CADENA: {
            const char* s = (tiene_init && init.valor) ? (const char*)init.valor : "";
            cg_arm64_decl_global_string(H, nombre, s, tiene_init);
        } break;
        default: break;
    }
}
// -----------------------------------------------------------

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
    
        TipoRetorno val = ejecutar(node->right, e);
        const char* t = (node->left && node->left->value) ? node->left->value : NULL;
        TipoRetorno def = declarar(t);        
        s->tipo = def.tipo;                   

        if (val.tipo == def.tipo) {

            s->valor = val.valor;
            setVar(e, s);


            emitir_decl_arm(s->id, def, 1, val);

            return;
        }

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
                    s->valor = p; setVar(e, s);


                    TipoRetorno init_conv; init_conv.tipo = TIPO_ENTERO; init_conv.valor = p; init_conv.control = CTRL_NORMAL;
                    emitir_decl_arm(s->id, (TipoRetorno){.tipo=TIPO_ENTERO,.valor=NULL,.control=CTRL_NORMAL}, 1, init_conv);

                    return;
                }
            } else if (def.tipo == TIPO_DECIMAL) {
                
                float *p = (float*)malloc(sizeof(float));
                if (!p){ fprintf(stderr,"OOM\n"); free(s); return; }
                if      (val.tipo == TIPO_CHAR)   *p = (float)(*(char*)val.valor);
                else if (val.tipo == TIPO_ENTERO) *p = (float)(*(int*)val.valor);
                else       *p = *(float*)val.valor;
                s->valor = p; setVar(e, s);


                TipoRetorno init_conv; init_conv.tipo = TIPO_DECIMAL; init_conv.valor = p; init_conv.control = CTRL_NORMAL;
                emitir_decl_arm(s->id, (TipoRetorno){.tipo=TIPO_DECIMAL,.valor=NULL,.control=CTRL_NORMAL}, 1, init_conv);
                
                return;
            } else if (def.tipo == TIPO_DOUBLE) {
               
                double *p = (double*)malloc(sizeof(double));
                if (!p){ fprintf(stderr,"OOM\n"); free(s); return; }
                if      (val.tipo == TIPO_CHAR)    *p = (double)(*(char*)val.valor);
                else if (val.tipo == TIPO_ENTERO)  *p = (double)(*(int*)val.valor);
                else if (val.tipo == TIPO_DECIMAL) *p = (double)(*(float*)val.valor);
                else        *p = *(double*)val.valor;
                s->valor = p; setVar(e, s);

                
                TipoRetorno init_conv; init_conv.tipo = TIPO_DOUBLE; init_conv.valor = p; init_conv.control = CTRL_NORMAL;
                emitir_decl_arm(s->id, (TipoRetorno){.tipo=TIPO_DOUBLE,.valor=NULL,.control=CTRL_NORMAL}, 1, init_conv);
                
                return;
            }
         
        }

        printf("Error: Tipos no coinciden\n");
        errores_add("Error: Tipos no coinciden\n");
        free(s);
        return;
    } else {
    
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

        emitir_decl_arm(s->id, def, 0, def);

    }
}
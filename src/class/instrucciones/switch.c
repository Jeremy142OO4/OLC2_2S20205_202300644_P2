#include "switch.h"
#include <string.h>

void recorrerCasos(struct ASTNode* nodo, struct entorno* entorno, TipoRetorno val, int* coincidio, struct ASTNode** def) {
    if (!nodo) return;

    if (strcmp(nodo->kind, "link") == 0) {

        recorrerCasos(nodo->left, entorno, val, coincidio, def);

        recorrerCasos(nodo->right, entorno, val, coincidio, def);
        return;
    }

    if (strcmp(nodo->kind, "case") == 0) {
        if (nodo->left == NULL) {

            if (!*def) *def = nodo->right;
        } else if (!*coincidio) {
            TipoRetorno valorCase = ejecutar(nodo->left, entorno);
            int iguales = 0;

            if (val.tipo == valorCase.tipo) {
                if (val.tipo == TIPO_ENTERO)
                    iguales = (*(int*)val.valor == *(int*)valorCase.valor);
                else if (val.tipo == TIPO_CADENA)
                    iguales = strcmp((char*)val.valor, (char*)valorCase.valor) == 0;
            }

            if (iguales) {
                ejecutar(nodo->right, entorno);
                *coincidio = 1;
            }
        }
    }
}

void ejecutarSwitch(struct ASTNode* nodo, struct entorno* entorno) {
    if (!nodo) return;

    TipoRetorno var = ejecutar(nodo->left, entorno);

    int coincidio = 0;
    struct ASTNode* def = NULL;
    struct entorno* entorno_switch = nuevo_entorno(entorno);
    recorrerCasos(nodo->right, entorno_switch, var, &coincidio, &def);

    if (!coincidio && def) {
        ejecutar(def, entorno_switch);  
    }

    
}

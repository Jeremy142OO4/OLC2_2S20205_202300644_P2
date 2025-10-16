#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "incremento_decremento.h"
#include "../expresiones/ejecutar.h" 

TipoRetorno ejecutarIncremento_Decremento(struct ASTNode* node, struct entorno* e) {
    TipoRetorno res = { NULL, TIPO_NULO ,CTRL_NORMAL};
    if (!node || !node->left || !node->left->value) {
        printf("Error: asignación con nodo inválido\n");
        return res;
    }

    struct symbol *s = getVar(e, node->left->value);
    //printf("direccion %p nombre:'%s'\n", (void *)s, node->left->value);

    if (!s) {
        printf("Error: variable '%s' no definida para asignación\n", node->left->value);
        return res;
    }

    //printf("Asignando a variable: %s\n", s->id);

    if (s->constante == 1) {
        printf("Error: no se puede asignar a constante '%s'\n", s->id);
        return res;
    }

    TipoRetorno val = ejecutar(node->right, e);

    if (strcmp(node->value, "++") == 0) {
        int* p = malloc(sizeof(int));
        *p = 1;
        int *salida = malloc(sizeof(int));
        *salida = (*(int*)s->valor) + *p;
        s->valor = salida; 
        res.valor = salida; 
        res.tipo = TIPO_ENTERO;
        
    }
    else if (strcmp(node->value, "--") == 0) {
        int* p = malloc(sizeof(int));
        *p = 1;
        int *salida = malloc(sizeof(int));
        *salida = (*(int*)s->valor) - *p;
        s->valor = salida; 
        res.valor = salida; 
        res.tipo = TIPO_ENTERO;
    }
    else {
        printf("Error: operador de asignación no soportado '%s'\n", node->value);
    }

    return res;
}

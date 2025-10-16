#include "declarar_funcion.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


static Tipo conseguir_tipo(struct ASTNode* tipo) {
    if (!tipo || !tipo->value) return TIPO_NULO; 
    if (strcmp(tipo->value, "int")    == 0) return TIPO_ENTERO;
    if (strcmp(tipo->value, "float")  == 0) return TIPO_DECIMAL;
    if (strcmp(tipo->value, "string") == 0) return TIPO_CADENA;
    if (strcmp(tipo->value, "bool")   == 0) return TIPO_BOOLEANO;
    if (strcmp(tipo->value, "char")   == 0) return TIPO_CHAR;
    if (strcmp(tipo->value, "double") == 0) return TIPO_DOUBLE;
    return TIPO_NULO;
}


static int contar_parametros(struct ASTNode* parametros) {
    int n = 0;
    while (parametros) {
        n++;
        parametros = parametros->right; 
    }
    return n;
}

TipoRetorno ejecutarDeclararFuncion(struct ASTNode* node, struct entorno* e) {

    if (!node || !node->value) {
        
        return (TipoRetorno){ NULL, TIPO_NULO, CTRL_NORMAL };
    }

    const char* nombre = node->value;
    struct ASTNode* parametros = node->left;
    struct ASTNode* link   = node->right;       
    struct ASTNode* tipo   = link ? link->left  : NULL;
    struct ASTNode* instrucciones = link ? link->right : NULL;

    funcion* f = (funcion*)malloc(sizeof(funcion));
    

    f->id       = strdup(nombre);
    f->retorno  = conseguir_tipo(tipo);
    f->parametros   = parametros;                
    f->num_parametros   = contar_parametros(parametros);  
    f->instrucciones   = instrucciones;
    f->siguiente = NULL;

    
    setFunc(e, f);

    return (TipoRetorno){ NULL, TIPO_NULO, CTRL_NORMAL };
}

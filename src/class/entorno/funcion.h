#ifndef FUNCION_H
#define FUNCION_H

#include "../tipos.h"
#include "../../ast/ast.h"
#include "../entorno/entorno.h"

typedef struct funcion {
    char* id;                   
    Tipo  retorno;              
    struct ASTNode* parametros;     
    int num_parametros;                 
    struct ASTNode* instrucciones;    
    struct funcion* siguiente; 
} funcion;


#endif

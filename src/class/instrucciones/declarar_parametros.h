#ifndef DECLARAR_PARAMETROS_H
#define DECLARAR_PARAMETROS_H

#include "../tipos.h"      
#include "../entorno/entorno.h"    
#include "../../ast/ast.h"  
#include "../expresiones/ejecutar.h"  

void ejecutarDeclararParametros(struct ASTNode* node, struct entorno* e);

#endif

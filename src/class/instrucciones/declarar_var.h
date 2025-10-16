#ifndef DECLARAR_VAR_H
#define DECLARAR_VAR_H

#include "../tipos.h"      
#include "../entorno/entorno.h"    
#include "../../ast/ast.h"  
#include "../expresiones/ejecutar.h"  

void ejecutarDeclarar(struct ASTNode* node, struct entorno* e, int constante);

#endif

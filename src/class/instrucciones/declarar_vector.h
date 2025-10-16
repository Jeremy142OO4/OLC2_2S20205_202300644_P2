#ifndef DECLARAR_VECTOR_H
#define DECLARAR_VECTOR_H

#include "../tipos.h"      
#include "../entorno/entorno.h"    
#include "../../ast/ast.h"  
#include "../expresiones/ejecutar.h"  

void ejecutarDeclararVector(struct ASTNode* node, struct entorno* e);

#endif

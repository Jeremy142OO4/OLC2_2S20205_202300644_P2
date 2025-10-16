#ifndef WHILE_H
#define WHILE_H

#include "../tipos.h"  
#include "../entorno/entorno.h"    
#include "../../ast/ast.h"  
#include "../expresiones/ejecutar.h"  

void ejecutarWhile(struct ASTNode* node, struct entorno* entorno);

#endif
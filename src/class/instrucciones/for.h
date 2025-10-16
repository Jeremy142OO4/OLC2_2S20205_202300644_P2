#ifndef FOR_H
#define FOR_H

#include "../tipos.h"  
#include "../entorno/entorno.h"    
#include "../../ast/ast.h"  
#include "../expresiones/ejecutar.h"  

void ejecutarFor(struct ASTNode* node, struct entorno* entorno);

#endif
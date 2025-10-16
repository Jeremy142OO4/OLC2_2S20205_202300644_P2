#ifndef SWITCH_H
#define SWITCH_H   

#include "../tipos.h"  
#include "../entorno/entorno.h"    
#include "../../ast/ast.h"  
#include "../expresiones/ejecutar.h"  

void ejecutarSwitch(struct ASTNode* node, struct entorno* e);

#endif
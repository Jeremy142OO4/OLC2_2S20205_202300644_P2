#ifndef INDEXOF_H
#define INDEXOF_H

#include "../tipos.h"    
#include "../entorno/entorno.h"   
#include "../../ast/ast.h" 

TipoRetorno ejecutarIndexOf(struct ASTNode* node, struct entorno* e);

#endif
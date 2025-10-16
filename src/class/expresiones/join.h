#ifndef JOIN_H
#define JOIN_H

#include "../tipos.h"    
#include "../entorno/entorno.h"   
#include "../../ast/ast.h" 

TipoRetorno ejecutarJoin(struct ASTNode* node, struct entorno* e);

#endif
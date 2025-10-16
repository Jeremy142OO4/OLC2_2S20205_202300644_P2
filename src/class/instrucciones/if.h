#ifndef IF_H
#define IF_H

#include "../tipos.h"  
#include "../entorno/entorno.h"    
#include "../../ast/ast.h"  
#include "../expresiones/ejecutar.h"  

TipoRetorno ejecutarIf(struct ASTNode* node, struct entorno* e);

#endif
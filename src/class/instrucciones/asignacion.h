#ifndef ASIGNACION_H
#define ASIGNACION_H

#include "../../ast/ast.h"    
#include "../entorno/entorno.h"     
#include "../tipos.h"      


TipoRetorno ejecutarAsignacion(struct ASTNode* node, struct entorno* e);

#endif

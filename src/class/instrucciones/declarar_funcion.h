#ifndef DECLARAR_FUNCION_H
#define DECLARAR_FUNCION_H

#include "../tipos.h"
#include "../entorno/entorno.h"
#include "../../ast/ast.h"
#include "../expresiones/ejecutar.h"   
#include "../entorno/funcion.h"        


TipoRetorno ejecutarDeclararFuncion(struct ASTNode* node, struct entorno* e);

#endif

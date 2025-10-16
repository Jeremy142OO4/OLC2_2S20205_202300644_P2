#ifndef RETORNAR_H
#define RETORNAR_H

#include "../tipos.h"
#include "../entorno/entorno.h"
#include "../../ast/ast.h"
#include "../expresiones/ejecutar.h"

TipoRetorno ejecutarRetornar(struct ASTNode* node, struct entorno* e);

#endif
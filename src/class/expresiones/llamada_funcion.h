#ifndef LLAMADA_FUNCION_H
#define LLAMADA_FUNCION_H

#include "../tipos.h"
#include "../entorno/entorno.h"
#include "../../ast/ast.h"
#include "../expresiones/ejecutar.h"

TipoRetorno ejecutarLlamadaFuncion(struct ASTNode* node, struct entorno* e);

#endif
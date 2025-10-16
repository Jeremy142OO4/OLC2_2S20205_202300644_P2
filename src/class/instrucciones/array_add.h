#ifndef ARRAY_ADD_H
#define ARRAY_ADD_H

#include "../tipos.h"
#include "../entorno/entorno.h"
#include "../../ast/ast.h"

void ejecutarArrayAdd(struct ASTNode* node, struct entorno* e);

#endif
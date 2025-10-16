#ifndef VECTOR_INDEX_H
#define VECTOR_INDEX_H

#include "../tipos.h"
#include "../entorno/entorno.h"
#include "../../ast/ast.h"
#include "./ejecutar.h"
#include <string.h> // memcpy


TipoRetorno ejecutarVectorGet(struct ASTNode* indexNode, struct entorno* e);


void ejecutarVectorSet(struct ASTNode* indexNode, struct ASTNode* valueExpr, struct entorno* e);

#endif

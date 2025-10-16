#ifndef EJECUTOR_H
#define EJECUTOR_H

#include "../ast/ast.h"
#include "../entorno/entorno.h"
#include "../tipos.h"

TipoRetorno ejecutar(struct ASTNode* node, struct entorno* entorno);

#endif

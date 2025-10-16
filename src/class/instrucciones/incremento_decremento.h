#ifndef INCREMENTO_DECREMENTO_H
#define INCREMENTO_DECREMENTO_H


#include "../../ast/ast.h"    
#include "../entorno/entorno.h"     
#include "../tipos.h"      


TipoRetorno ejecutarIncremento_Decremento(struct ASTNode* node, struct entorno* e);

#endif
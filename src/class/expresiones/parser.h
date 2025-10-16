#ifndef PARSER_H
#define PARSER_H
#include "../tipos.h"     
#include "../entorno/entorno.h"   

TipoRetorno ejecutarParser(struct ASTNode* node, struct entorno* e);

#endif

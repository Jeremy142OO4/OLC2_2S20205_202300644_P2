#ifndef EXPRESION_H
#define EXPRESION_H

#include "tipos.h"

struct entorno; 

typedef struct expresion {
    TipoRetorno (*ejecutar)(struct expresion* self, struct entorno* entorno);
} expresion;

#endif

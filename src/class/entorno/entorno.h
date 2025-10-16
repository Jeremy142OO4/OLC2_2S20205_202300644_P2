#ifndef ENTORNO_H
#define ENTORNO_H

#include "symbol.h"
#include "funcion.h"

#define HASH_SIZE 128

typedef struct entorno {
    struct entorno* anterior; 
    symbol* tabla[HASH_SIZE]; // HashMap
    funcion* funs[HASH_SIZE];
} entorno;

entorno* nuevo_entorno(entorno* anterior);

void setVar(entorno* entorno, symbol* simbolo);
symbol* getVar(entorno* entorno, const char* id);
symbol* validar_var(entorno* entorno, const char* id);

void setFunc(entorno* entorno, funcion* funcion);
funcion* getFunc(entorno* entorno, const char* id);

void exportar_tabla(entorno* e, const char* archivo, const char* entorno);

#endif

#include "entorno.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

unsigned int hash(const char* str) {
    unsigned int h = 0;
    while (*str) h = (h << 5) + *str++;
    return h % HASH_SIZE;
}



entorno* nuevo_entorno(entorno* anterior) {
    entorno* e = malloc(sizeof(entorno));
    e->anterior = anterior;
    for (int i = 0; i < HASH_SIZE; i++) { e->tabla[i]=NULL; e->funs[i]=NULL; }
    return e;
}

void setVar(entorno* entorno, symbol* simbolo) {
    unsigned int h = hash(simbolo->id);
    symbol* actual = entorno->tabla[h];
    while (actual) {
        if (strcmp(actual->id, simbolo->id) == 0) {
            actual->valor = simbolo->valor;
            actual->tipo = simbolo->tipo;
            return;
        }
        actual = actual->siguiente;
    }

    simbolo->siguiente = entorno->tabla[h];
    entorno->tabla[h] = simbolo;
}

symbol* getVar(entorno* entorno, const char* id) {
    while (entorno) {
        unsigned int h = hash(id);
        symbol* actual = entorno->tabla[h];
        while (actual) {
            if (strcmp(actual->id, id) == 0) {
                return actual;
            }
            actual = actual->siguiente;
        }
        entorno = entorno->anterior; 
    }
    return NULL;
}

symbol* validar_var(entorno* entorno, const char* id) {
    
    unsigned int h = hash(id);
    symbol* actual = entorno->tabla[h];
    while (actual) {
        if (strcmp(actual->id, id) == 0) {
            return actual;
        }
        actual = actual->siguiente;
    }
    
    
    return NULL;
}

void setFunc(entorno* entorno, funcion* f) {
    if (!entorno || !f || !f->id) return;
    unsigned int h = hash(f->id);
    funcion* actual = entorno->funs[h];

    while (actual) {
        if (strcmp(actual->id, f->id) == 0) {
            actual->retorno = f->retorno;
            actual->parametros  = f->parametros;
            actual->num_parametros  = f->num_parametros;
            actual->instrucciones  = f->instrucciones;
            return;
        }
        actual = actual->siguiente;
    }

    f->siguiente = entorno->funs[h];
    entorno->funs[h] = f;
}

funcion* getFunc(entorno* entorno, const char* id) {
    while (entorno) {
        unsigned int h = hash(id);
        funcion* actual = entorno->funs[h];
        while (actual) {
            if (strcmp(actual->id, id) == 0) {
                return actual;
            }
            actual = actual->siguiente;
        }
        entorno = entorno->anterior; 
    }
    return NULL;
}

const char* nombre_tipo(Tipo t) {
    switch (t) {
        case TIPO_NULO:     return "NULO";
        case TIPO_ENTERO:   return "ENTERO";
        case TIPO_DECIMAL:  return "DECIMAL";
        case TIPO_CADENA:   return "CADENA";
        case TIPO_BOOLEANO: return "BOOLEANO";
        case TIPO_CHAR:     return "CHAR";
        case TIPO_DOUBLE:   return "DOUBLE";
        default:            return "DESCONOCIDO";
    }
}

void exportar_tabla(entorno* e, const char* archivo, const char* entorno) {
    FILE* f = fopen(archivo, "w");
    if (!f) {
        perror("Error al abrir archivo");
        return;
    }

    // Escribir encabezados
    fprintf(f, "ID,Tipo,Entorno,Valor\n");

    for (int i = 0; i < HASH_SIZE; i++) {
        symbol* s = e->tabla[i];
        while (s) {
            fprintf(f, "%s,%s,%s,", 
                    s->id, 
                    nombre_tipo(s->tipo), 
                    entorno);

            if (s->valor) {
                switch (s->tipo) {
                    case TIPO_ENTERO:
                        fprintf(f, "%d", *((int*)s->valor));
                        break;
                    case TIPO_DECIMAL:
                        fprintf(f, "%f", *((float*)s->valor));
                        break;
                    case TIPO_DOUBLE:
                        fprintf(f, "%lf", *((double*)s->valor));
                        break;
                    case TIPO_CHAR:
                        fprintf(f, "%c", *((char*)s->valor));
                        break;
                    case TIPO_BOOLEANO:
                        fprintf(f, "%s", (*((int*)s->valor) ? "true" : "false"));
                        break;
                    case TIPO_CADENA:
                        fprintf(f, "\"%s\"", (char*)s->valor);
                        break;
                    case TIPO_NULO:
                        fprintf(f, "null");
                        break;
                    default:
                        fprintf(f, "??");
                        break;
                }
            } else {
                fprintf(f, "null");
            }

            fprintf(f, "\n");
            s = s->siguiente;
        }
    }

    fclose(f);
}
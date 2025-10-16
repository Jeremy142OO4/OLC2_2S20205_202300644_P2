
#ifndef TIPOS_H
#define TIPOS_H

typedef enum {
    TIPO_NULO,
    TIPO_ENTERO,
    TIPO_DECIMAL,
    TIPO_CADENA,
    TIPO_BOOLEANO,
    TIPO_CHAR,
    TIPO_DOUBLE
} Tipo;

typedef enum {
    CTRL_NORMAL,
    CTRL_BREAK,
    CTRL_CONTINUE,
    CTRL_RETURN
} ControlFlujo;

typedef struct {
    void* valor;
    Tipo  tipo;
    ControlFlujo control; 
} TipoRetorno;

#endif


#include "break.h"
#include <stdio.h>
#include <stdlib.h>

TipoRetorno ejecutarBreak() {
    //printf("Ejecutando break\n");
    TipoRetorno res;
    res.valor = NULL;
    res.tipo = TIPO_NULO;
    res.control = CTRL_BREAK;   
    //printf("Control flujo en break: %d\n", res.control);
    return res;
}

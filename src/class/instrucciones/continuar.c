#include "continuar.h"
#include <stdio.h>

TipoRetorno ejecutarContinuar() {
    TipoRetorno res;
    res.valor = NULL;
    res.tipo = TIPO_NULO;
    res.control = CTRL_CONTINUE;
    return res;
}

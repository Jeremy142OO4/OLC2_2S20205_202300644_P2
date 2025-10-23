#include <stdio.h>
#include "imprimir.h"
#include "../asm/imprimir_asm.h"
void ejecutarImprimir(TipoRetorno valor) {
    switch (valor.tipo) {
        case TIPO_ENTERO:
            printf("%d\n", *((int *)valor.valor));
            break;
        case TIPO_DECIMAL:
            printf("%.2f\n", *((float *)valor.valor));
            break;
        case TIPO_CADENA:
            printf("%s\n", (char *)valor.valor);
            break;
        case TIPO_BOOLEANO:
            if (*((int *)valor.valor) == 1) {
                printf("true\n");
            } else {
                printf("false\n");
            }
            break;
        case TIPO_CHAR:
            printf("%c\n", *((char *)valor.valor));
            break;
        case TIPO_DOUBLE:
            
            printf("%.5f\n", *((double *)valor.valor));
            break;
        default:
            printf("null\n");
            break;
    }

}

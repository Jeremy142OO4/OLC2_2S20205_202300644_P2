#include <stdio.h>
#include <stdlib.h>
#include "imprimir.h"
#include "../utils/num_to_str.h"
#include "../utils/arm_codegen_singleton.h"

// helper para copiar cadenas (sin strdup, por si no lo usas)
static char* dup_cstr(const char* s){
    if (!s) { char* z=malloc(1); z[0]='\0'; return z; }
    size_t n=0; while(s[n]) n++;
    char* r = (char*)malloc(n+1);
    for(size_t i=0;i<n;i++) r[i]=s[i];
    r[n]='\0';
    return r;
}

void ejecutarImprimir(TipoRetorno valor) {
    char* s = NULL;

    switch (valor.tipo) {
        case TIPO_ENTERO: {
            long long v = *((int*)valor.valor);  
            s = nts_i64(v);
            break;
        }
        case TIPO_DECIMAL: {
            double v = (double)(*((float*)valor.valor));
            s = nts_f64(v, 2);                    
            break;
        }
        case TIPO_DOUBLE: {
            double v = *((double*)valor.valor);
            s = nts_f64(v, 5);                     
            break;
        }
        case TIPO_CADENA: {
            s = dup_cstr((const char*)valor.valor);
            break;
        }
        case TIPO_BOOLEANO: {
            int b = *((int*)valor.valor);
            s = nts_bool(b);
            break;
        }
        case TIPO_CHAR: {                       
            unsigned char c = *((unsigned char*)valor.valor);
            s = nts_char(c);
            break;
        }
        default: {
            s = dup_cstr("null");
            break;
        }
    }


    fputs(s, stdout);
    fputc('\n', stdout);


    arm_codegen_emit_println_from_cstring(s);

    free(s);
}

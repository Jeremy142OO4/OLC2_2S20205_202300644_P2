#include "imprimir_asm.h"
#include <string.h>

static int decl_base = 0;
static void asegurar_base(void){
    if (decl_base) return;
    asm_decl_str("str_true",  "true");
    asm_decl_str("str_false", "false");
    asm_decl_str("str_nl",    "\n");
    decl_base = 1;
}

static void asm_nl(void){
    asegurar_base();
    asm_print_inline_lbl("str_nl", 1);
}

void imprimirASM(TipoRetorno valor){
    switch (valor.tipo) {
        case TIPO_CADENA: {
            const char *s = (const char*)valor.valor;
            asm_print_cstr_inline(s);
            asm_nl();
            break;
        }
        case TIPO_ENTERO: {
            long long v = (long long)(*((int*)valor.valor));
            asm_cargar_x_imm("x2", (unsigned long long)v);
            asm_imprimir_entero_inline_desde_x2();
            asm_nl();
            break;
        }
        case TIPO_BOOLEANO: {
            asegurar_base();
            int b = *((int*)valor.valor) ? 1 : 0;
            if (b) asm_print_inline_lbl("str_true", 4);
            else   asm_print_inline_lbl("str_false", 5);
            asm_nl();
            break;
        }
        case TIPO_CHAR: {
            char c = *((char*)valor.valor);
            char buf[2] = { c, 0 };
            asm_print_cstr_inline(buf);
            asm_nl();
            break;
        }
        case TIPO_DECIMAL:
        case TIPO_DOUBLE: {
            // pendiente: formato fijo; por ahora "0.0" (sin mezclar libc)
            asm_print_cstr_inline("0.0");
            asm_nl();
            break;
        }
        default: {
            asm_print_cstr_inline("null");
            asm_nl();
            break;
        }
    }
}


#ifndef ASM_DECL_H
#define ASM_DECL_H

#include "../class/tipos.h"  
#include "../utils/asm_gen.h"           

void asm_decl_init(void);
void asm_decl_close(void);

const char* asm_decl_global(const char* id, Tipo tipo, void* valor_inicial);


const char* asm_label_of(const char* id);

#endif

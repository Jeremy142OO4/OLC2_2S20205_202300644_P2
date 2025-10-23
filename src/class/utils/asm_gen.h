#ifndef ASM_GEN_H
#define ASM_GEN_H

#include <stddef.h>

// Sesión .s (si aún no los tienes con estos nombres)
int  asm_abrir(const char *ruta);
void asm_cerrar(void);

void asm_prog_inicio(void);
void asm_prog_fin_exit0(void);

void asm_texto(void);
void asm_datos(void);
void asm_ro(void);
void asm_glob(const char *nombre);
void asm_etq(const char *nombre);
void asm_ln(const char *s);
void asm_fmt(const char *fmt, ...);

// Cadenas
void asm_decl_str(const char *etq, const char *txt);
void asm_print_inline_lbl(const char *etq, size_t len);
void asm_print_cstr_inline(const char *txt);

// Enteros → texto (inline)
void asm_cargar_x_imm(const char *xr, unsigned long long v);
void asm_imprimir_entero_inline_desde_x2(void);


void asm_decl_glob_word(const char *lbl, unsigned val);              
void asm_decl_glob_quad(const char *lbl, unsigned long long val);    
void asm_decl_glob_byte(const char *lbl, unsigned char val);        
void asm_decl_glob_asciz(const char *lbl, const char *txt);    

#endif

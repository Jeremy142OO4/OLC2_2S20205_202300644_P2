#include "asm_gen.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static FILE *g = NULL;
static int g_inicio_emitido = 0; // para no repetir _start
static int g_str_id = 0;

// abrir/cerrar
int asm_abrir(const char *ruta){
    g_inicio_emitido = 0;
    g_str_id = 0;
    g = fopen(ruta, "w");
    return g ? 0 : -1;
}
void asm_cerrar(void){ if (g){ fclose(g); g=NULL; } }

// helpers básicos (si alguno falta):
void asm_ln(const char *s){ if (g){ fputs(s, g); fputc('\n', g); } }
void asm_fmt(const char *fmt, ...){
    if (!g) return; va_list ap; va_start(ap, fmt); vfprintf(g, fmt, ap); va_end(ap);
}
void asm_texto(void){ asm_ln("    .section .text"); }
void asm_datos(void){ asm_ln("    .section .data"); }
void asm_ro(void){    asm_ln("    .section .rodata"); }
void asm_glob(const char *nombre){ asm_fmt("    .global %s\n", nombre); }
void asm_etq(const char *nombre){ asm_fmt("%s:\n", nombre); }

void asm_prog_inicio(void){
    if (g_inicio_emitido) return;
    asm_texto();
    asm_glob("_start");
    asm_ln("    .p2align 2");
    asm_etq("_start");
    g_inicio_emitido = 1;
}
void asm_prog_fin_exit0(void){
    asm_texto();
    asm_ln("    mov     x0, #0");   // exit(0)
    asm_ln("    mov     x8, #93");  // SYS_exit
    asm_ln("    svc     #0");
}

// --- strings ---
static void esc(FILE *fp, const char *s){
    fputc('"', fp);
    for (const unsigned char *p=(const unsigned char*)s; *p; ++p){
        unsigned char c=*p;
        if (c=='"' || c=='\\') { fputc('\\', fp); fputc(c, fp); }
        else if (c=='\n') fputs("\\n", fp);
        else if (c=='\t') fputs("\\t", fp);
        else if (c>=32 && c<127) fputc(c, fp);
        else fprintf(fp, "\\x%02X", (unsigned)c);
    }
    fputc('"', fp);
}

void asm_decl_str(const char *etq, const char *txt){
    asm_ro(); asm_fmt("%s:        .ascii ", etq); if (g) esc(g, txt); asm_ln("");
}
void asm_print_inline_lbl(const char *etq, size_t len){
    asm_prog_inicio(); asm_texto();
    asm_fmt("    adrp    x0, %s\n", etq);
    asm_fmt("    add     x0, x0, :lo12:%s\n", etq);
    asm_ln("    mov     x8, #64");          // write
    asm_ln("    mov     x1, x0");           // buf
    asm_fmt("    mov     x2, #%zu\n", len); // len
    asm_ln("    mov     x0, #1");           // fd=1
    asm_ln("    svc     #0");
}
void asm_print_cstr_inline(const char *txt){
    char etq[64]; snprintf(etq, sizeof(etq), "str_%d", g_str_id++);
    asm_decl_str(etq, txt);
    asm_print_inline_lbl(etq, strlen(txt));
}

// --- enteros ---
void asm_cargar_x_imm(const char *xr, unsigned long long v){
    unsigned long long w0 =  v        & 0xFFFFULL;
    unsigned long long w1 = (v>>16)   & 0xFFFFULL;
    unsigned long long w2 = (v>>32)   & 0xFFFFULL;
    unsigned long long w3 = (v>>48)   & 0xFFFFULL;
    asm_fmt("    movz    %s, #%llu\n", xr, w0);
    if (w1) asm_fmt("    movk    %s, #%llu, lsl #16\n", xr, w1);
    if (w2) asm_fmt("    movk    %s, #%llu, lsl #32\n", xr, w2);
    if (w3) asm_fmt("    movk    %s, #%llu, lsl #48\n", xr, w3);
}

// x2 → ascii → write(1, ptr, len)  (inline; sin bl)
void asm_imprimir_entero_inline_desde_x2(void){
    asm_prog_inicio(); asm_texto();
    asm_ln("    sub     sp, sp, #64");
    asm_ln("    mov     w3, #0");
    asm_ln("    cbnz    x2, 1f");
    asm_ln("    mov     x9, sp");
    asm_ln("    mov     w4, #'0'");
    asm_ln("    strb    w4, [x9]");
    asm_ln("    mov     x8, #64");      // write
    asm_ln("    mov     x0, #1");
    asm_ln("    mov     x1, x9");
    asm_ln("    mov     x2, #1");
    asm_ln("    svc     #0");
    asm_ln("    add     sp, sp, #64");
    asm_ln("    b       5f");
    asm_ln("1:");
    asm_ln("    cmp     x2, #0");
    asm_ln("    bge     2f");
    asm_ln("    neg     x2, x2");
    asm_ln("    mov     w3, #1");
    asm_ln("2:");
    asm_ln("    add     x10, sp, #64");
    asm_ln("    mov     x11, x10");
    asm_ln("    mov     x5, #10");
    asm_ln("3:");
    asm_ln("    udiv    x7, x2, x5");
    asm_ln("    mul     x12, x7, x5");
    asm_ln("    sub     x8, x2, x12");
    asm_ln("    add     w8, w8, #'0'");
    asm_ln("    sub     x11, x11, #1");
    asm_ln("    strb    w8, [x11]");
    asm_ln("    mov     x2, x7");
    asm_ln("    cbnz    x2, 3b");
    asm_ln("    cbz     w3, 4f");
    asm_ln("    sub     x11, x11, #1");
    asm_ln("    mov     w8, #'-'");
    asm_ln("    strb    w8, [x11]");
    asm_ln("4:");
    asm_ln("    sub     x2, x10, x11");
    asm_ln("    mov     x8, #64");
    asm_ln("    mov     x0, #1");
    asm_ln("    mov     x1, x11");
    asm_ln("    svc     #0");
    asm_ln("    add     sp, sp, #64");
    asm_ln("5:");
}

void asm_decl_glob_word(const char *lbl, unsigned val){
    asm_datos();
    asm_fmt("%s:\n", lbl);
    asm_ln("    .p2align 2");
    asm_fmt("    .word %u\n", val);
}

void asm_decl_glob_quad(const char *lbl, unsigned long long val){
    asm_datos();
    asm_fmt("%s:\n", lbl);
    asm_ln("    .p2align 3");
    asm_fmt("    .quad %llu\n", (unsigned long long)val);
}

void asm_decl_glob_byte(const char *lbl, unsigned char val){
    asm_datos();
    asm_fmt("%s:\n", lbl);
    asm_fmt("    .byte %u\n", (unsigned)val);
}

void asm_decl_glob_asciz(const char *lbl, const char *txt){
    asm_ro();
    asm_fmt("%s:        .asciz ", lbl);
    if (g) esc(g, txt ? txt : "");
    asm_ln("");
}
#include "codegen_arm64.h"

// Evitar headers estándar: prototipos mínimos
void* malloc(unsigned long);
void  free(void*);

#ifndef MY_NULL
#define MY_NULL ((void*)0)
#endif

static int P(CodegenARM64 *cg, const char *s){ return em_puts(cg->em, s); }
static int C(CodegenARM64 *cg, char ch)      { return em_putc(cg->em, ch); }
static int U(CodegenARM64 *cg, u64 v)        { return em_putu64(cg->em, v); }

// --- helpers internos (sin libc) ---
static unsigned long my_strlen(const char *s){
    unsigned long n=0; if (!s) return 0; while(s[n]) n++; return n;
}
static char* my_strndup(const char* s, unsigned long n){
    char* p = (char*)malloc(n+1);
    for (unsigned long i=0;i<n;i++) p[i]=s[i];
    p[n]='\0'; return p;
}
static char* my_strdup(const char* s){ return my_strndup(s ? s : "", my_strlen(s?s:"")); }

void cg_arm64_nl(CodegenARM64 *cg){ C(cg, '\n'); }

void cg_arm64_comment(CodegenARM64 *cg, const char *txt){
    P(cg, "    // "); P(cg, txt); cg_arm64_nl(cg);
}

void cg_arm64_begin(CodegenARM64 *cg, Emitter *em){
    cg->em = em;
    cg->rodata_open = 0;
    cg->lit_count = 0;
    cg->g_count = 0;
    P(cg, ".text\n.global _start\n_start:\n");
}

void cg_arm64_emit_exit(CodegenARM64 *cg, int status){
    P(cg, "    mov x0, #"); U(cg, (u64)(unsigned)status); cg_arm64_nl(cg);
    P(cg, "    mov x8, #93\n");   // SYS_exit
    P(cg, "    svc #0\n");
}

int cg_arm64_intern_str(CodegenARM64 *cg, const char *asciz){
    if (cg->lit_count >= 256) return -1;
    unsigned long n = my_strlen(asciz);
    int id = cg->lit_count++;
    cg->lits[id].txt = my_strndup(asciz ? asciz : "", n); 
    cg->lits[id].len = n;
    return id;
}

void cg_arm64_write_literal_id(CodegenARM64 *cg, int id){
    P(cg, "    adrp x1, L_str_"); U(cg, (u64)id); cg_arm64_nl(cg);
    P(cg, "    add  x1, x1, :lo12:L_str_"); U(cg, (u64)id); cg_arm64_nl(cg);

    P(cg, "    adrp x2, L_str_"); U(cg, (u64)id); P(cg, "_len"); cg_arm64_nl(cg);
    P(cg, "    add  x2, x2, :lo12:L_str_"); U(cg, (u64)id); P(cg, "_len"); cg_arm64_nl(cg);
    P(cg, "    ldr  x2, [x2]\n");

    P(cg, "    mov  x0, #1\n");   // fd = 1
    P(cg, "    mov  x8, #64\n");  // write
    P(cg, "    svc  #0\n");
}

void cg_arm64_begin_rodata(CodegenARM64 *cg){
    if (cg->rodata_open) return;
    cg->rodata_open = 1;
    P(cg, "\n.section .rodata\n");
}

static void put_escaped(CodegenARM64 *cg, const char *s){
    for (; *s; ++s){
        char c = *s;
        if (c == '\"' || c == '\\') { C(cg,'\\'); C(cg,c); }
        else if (c == '\n') { P(cg,"\\n"); }
        else if (c == '\r') { P(cg,"\\r"); }
        else if (c == '\t') { P(cg,"\\t"); }
        else { C(cg,c); }
    }
}

void cg_arm64_flush_rodata(CodegenARM64 *cg){
    cg_arm64_begin_rodata(cg);
    for (int i=0;i<cg->lit_count;i++){
        P(cg,"L_str_"); U(cg,(u64)i); P(cg,": .ascii \"");
        put_escaped(cg, cg->lits[i].txt);
        P(cg,"\"\n");

        P(cg,"L_str_"); U(cg,(u64)i); P(cg,"_len: .quad ");
        U(cg,(u64)cg->lits[i].len);
        cg_arm64_nl(cg);
    }
    for (int i=0;i<cg->lit_count;i++){
        if (cg->lits[i].txt) { free(cg->lits[i].txt); cg->lits[i].txt = MY_NULL; }
    }
    cg->lit_count = 0;
}

int cg_arm64_print_literal(CodegenARM64 *cg, const char *asciz){
    int id = cg_arm64_intern_str(cg, asciz);
    if (id < 0) return -1;
    cg_arm64_write_literal_id(cg, id);
    return id;
}

void cg_arm64_print_nl(CodegenARM64 *cg){
    int id = cg_arm64_intern_str(cg, "\n");
    cg_arm64_write_literal_id(cg, id);
}

// ======== HEX helpers para emitir bits IEEE754 sin stdio ========
static void em_put_hex_prefix(Emitter* em){ em_write(em, "0x", 2); }

static void em_put_hex_nibble(Emitter* em, unsigned int nib){
    char c = (nib < 10) ? (char)('0' + nib) : (char)('A' + (nib-10));
    em_write(em, &c, 1);
}

static void em_put_hex_u64_digits(Emitter* em, unsigned long long v, int digits){
    em_put_hex_prefix(em);
    for (int i = digits-1; i >= 0; --i){
        unsigned int nib = (unsigned int)((v >> (i*4)) & 0xFULL);
        em_put_hex_nibble(em, nib);
    }
}
static void em_put_hex_u32(Emitter* em, unsigned int v){
    em_put_hex_u64_digits(em, (unsigned long long)v, 8);
}
static void em_put_hex_u64_full(Emitter* em, unsigned long long v){
    em_put_hex_u64_digits(em, v, 16);
}
// =================================================================

// ---- Declaraciones de variables (.data) ----
static int add_global(CodegenARM64 *cg, const char* name, CgType t){
    if (cg->g_count >= 256) return -1;
    int id = cg->g_count++;
    cg->globals[id].name    = my_strdup(name);
    cg->globals[id].type    = t;
    cg->globals[id].has_init= 0;
    cg->globals[id].i64     = 0;
    cg->globals[id].f64     = 0.0;
    cg->globals[id].str_id  = -1;
    return id;
}

int cg_arm64_decl_global_int(CodegenARM64 *cg, const char* name, long long v, int has_init){
    int id = add_global(cg, name, CG_T_INT);
    if (id<0) return -1;
    cg->globals[id].has_init = has_init;
    cg->globals[id].i64 = v;
    return id;
}
int cg_arm64_decl_global_bool(CodegenARM64 *cg, const char* name, int v, int has_init){
    int id = add_global(cg, name, CG_T_BOOL);
    if (id<0) return -1;
    cg->globals[id].has_init = has_init;
    cg->globals[id].i64 = v ? 1 : 0;
    return id;
}
int cg_arm64_decl_global_char(CodegenARM64 *cg, const char* name, int v, int has_init){
    int id = add_global(cg, name, CG_T_CHAR);
    if (id<0) return -1;
    cg->globals[id].has_init = has_init;
    cg->globals[id].i64 = v & 0xFF;
    return id;
}
int cg_arm64_decl_global_float(CodegenARM64 *cg, const char* name, float v, int has_init){
    int id = add_global(cg, name, CG_T_FLOAT);
    if (id<0) return -1;
    cg->globals[id].has_init = has_init;
    cg->globals[id].f64 = (double)v;
    return id;
}
int cg_arm64_decl_global_double(CodegenARM64 *cg, const char* name, double v, int has_init){
    int id = add_global(cg, name, CG_T_DOUBLE);
    if (id<0) return -1;
    cg->globals[id].has_init = has_init;
    cg->globals[id].f64 = v;
    return id;
}
int cg_arm64_decl_global_string(CodegenARM64 *cg, const char* name, const char* s, int has_init){
    int id = add_global(cg, name, CG_T_STRING);
    if (id<0) return -1;
    cg->globals[id].has_init = has_init;
    cg->globals[id].str_id = has_init ? cg_arm64_intern_str(cg, s?s:"") : -1;
    return id;
}

void cg_arm64_flush_data(CodegenARM64 *cg){
    if (cg->g_count == 0) return;
    P(cg, "\n.section .data\n");
    for (int i=0;i<cg->g_count;i++){

        P(cg, cg->globals[i].name); P(cg, ":\n");
        switch (cg->globals[i].type){
            case CG_T_INT:
            case CG_T_BOOL:
                P(cg, "    .word "); U(cg, (u64)(unsigned)cg->globals[i].i64); P(cg, "\n");
                break;
            case CG_T_CHAR:
                P(cg, "    .byte "); U(cg, (u64)(unsigned)cg->globals[i].i64); P(cg, "\n");
                break;
            case CG_T_FLOAT: {

                union { float f; unsigned int u; } u32;
                u32.f = (float)cg->globals[i].f64;
                P(cg, "    .word "); em_put_hex_u32(cg->em, u32.u); P(cg, "\n");
                break;
            }
            case CG_T_DOUBLE: {

                union { double d; unsigned long long u; } u64v;
                u64v.d = cg->globals[i].f64;
                P(cg, "    .quad "); em_put_hex_u64_full(cg->em, u64v.u); P(cg, "\n");
                break;
            }
            case CG_T_STRING:
                if (cg->globals[i].has_init && cg->globals[i].str_id >= 0){
                    P(cg, "    .quad L_str_"); U(cg, (u64)cg->globals[i].str_id); P(cg, "\n");
                } else {
                    P(cg, "    .quad 0\n");
                }
                break;
        }
        // libera nombre
        free(cg->globals[i].name); cg->globals[i].name = MY_NULL;
    }
    cg->g_count = 0;
}


void cg_arm64_load_int(CodegenARM64* cg, const char* label, int dst_wreg){
    P(cg, "    adrp x9, "); P(cg, label); cg_arm64_nl(cg);
    P(cg, "    add  x9, x9, :lo12:"); P(cg, label); cg_arm64_nl(cg);
    P(cg, "    ldr  w"); U(cg, (u64)dst_wreg); P(cg, ", [x9]\n");
}

void cg_arm64_store_int(CodegenARM64* cg, int src_wreg, const char* label){
    P(cg, "    adrp x9, "); P(cg, label); cg_arm64_nl(cg);
    P(cg, "    add  x9, x9, :lo12:"); P(cg, label); cg_arm64_nl(cg);
    P(cg, "    str  w"); U(cg, (u64)src_wreg); P(cg, ", [x9]\n");
}

void cg_arm64_store_int_imm(CodegenARM64* cg, const char* label, int value){
    P(cg, "    mov  w0, #"); U(cg, (u64)(unsigned)(value & 0xFFFFFFFFU)); cg_arm64_nl(cg);
    cg_arm64_store_int(cg, 0, label);
}

static void emit_movz_movk_w0(CodegenARM64* cg, unsigned int v){
    unsigned int lo =  v        & 0xFFFFU;
    unsigned int hi = (v >> 16) & 0xFFFFU;
    P(cg, "    movz w0, #"); U(cg, (u64)lo); cg_arm64_nl(cg);
    P(cg, "    movk w0, #"); U(cg, (u64)hi); P(cg, ", lsl #16\n");
}
static void emit_movz_movk_x0(CodegenARM64* cg, unsigned long long v){
    unsigned long long p0 =  v        & 0xFFFFULL;
    unsigned long long p1 = (v >> 16) & 0xFFFFULL;
    unsigned long long p2 = (v >> 32) & 0xFFFFULL;
    unsigned long long p3 = (v >> 48) & 0xFFFFULL;
    P(cg, "    movz x0, #"); U(cg, (u64)p0); cg_arm64_nl(cg);
    P(cg, "    movk x0, #"); U(cg, (u64)p1); P(cg, ", lsl #16\n");
    P(cg, "    movk x0, #"); U(cg, (u64)p2); P(cg, ", lsl #32\n");
    P(cg, "    movk x0, #"); U(cg, (u64)p3); P(cg, ", lsl #48\n");
}

// direccionar label en x9 (ADR + ADD)
static void emit_addr_x9(CodegenARM64* cg, const char* label){
    P(cg, "    adrp x9, "); P(cg, label); cg_arm64_nl(cg);
    P(cg, "    add  x9, x9, :lo12:"); P(cg, label); cg_arm64_nl(cg);
}

// ========== stores inmediatos por tipo ==========


void cg_arm64_store_bool_imm(CodegenARM64* cg, const char* label, int value01){
    P(cg, "    mov  w0, #"); U(cg, (u64)((value01!=0)?1:0)); cg_arm64_nl(cg);
    emit_addr_x9(cg, label);
    P(cg, "    str  w0, [x9]\n"); // bool en .word
}

void cg_arm64_store_char_imm(CodegenARM64* cg, const char* label, int ch){
    P(cg, "    mov  w0, #"); U(cg, (u64)(ch & 0xFF)); cg_arm64_nl(cg);
    emit_addr_x9(cg, label);
    P(cg, "    strb w0, [x9]\n"); // char en .byte
}

void cg_arm64_store_float_imm(CodegenARM64* cg, const char* label, float fval){
    union { float f; unsigned int u; } u32; u32.f = fval;
    emit_movz_movk_w0(cg, u32.u);  // bits del float en w0
    emit_addr_x9(cg, label);
    P(cg, "    str  w0, [x9]\n");  // 32 bits
}

void cg_arm64_store_double_imm(CodegenARM64* cg, const char* label, double dval){
    union { double d; unsigned long long u; } u64v; u64v.d = dval;
    emit_movz_movk_x0(cg, u64v.u); // bits del double en x0
    emit_addr_x9(cg, label);
    P(cg, "    str  x0, [x9]\n");  // 64 bits
}

void cg_arm64_store_string_lit(CodegenARM64* cg, const char* label, const char* s){
    // internamos literal y guardamos el puntero (address) en la variable .data
    int sid = cg_arm64_intern_str(cg, s?s:"");
    // x0 = &L_str_sid
    P(cg, "    adrp x0, L_str_"); U(cg, (u64)sid); cg_arm64_nl(cg);
    P(cg, "    add  x0, x0, :lo12:L_str_"); U(cg, (u64)sid); cg_arm64_nl(cg);
    emit_addr_x9(cg, label);
    P(cg, "    str  x0, [x9]\n"); // .quad
}
#pragma once
#include "sys_emitter.h"

typedef unsigned long long u64;


typedef enum {
    CG_T_INT, CG_T_FLOAT, CG_T_DOUBLE, CG_T_BOOL, CG_T_CHAR, CG_T_STRING
} CgType;

typedef struct CodegenARM64 {
    Emitter *em;
    int rodata_open;
    struct { char *txt; unsigned long len; } lits[256];
    int lit_count;


    struct {
        char *name;
        CgType type;
        int    has_init;
        long long i64;   // para int/bool/char
        double   f64;    // para float/double
        int str_id;      // literal de rodata para strings
    } globals[256];
    int g_count;
} CodegenARM64;

void cg_arm64_begin(CodegenARM64 *cg, Emitter *em);     // .text/_start
void cg_arm64_emit_exit(CodegenARM64 *cg, int status);

int  cg_arm64_intern_str(CodegenARM64 *cg, const char *asciz);
void cg_arm64_write_literal_id(CodegenARM64 *cg, int id);
int  cg_arm64_print_literal(CodegenARM64 *cg, const char *asciz); 
void cg_arm64_print_nl(CodegenARM64 *cg);  
void cg_arm64_begin_rodata(CodegenARM64 *cg);
void cg_arm64_flush_rodata(CodegenARM64 *cg);

// Helpers
void cg_arm64_nl(CodegenARM64 *cg);
void cg_arm64_comment(CodegenARM64 *cg, const char *txt);

 
// ---- NUEVO: declaraciones de variables ----
int  cg_arm64_decl_global_int   (CodegenARM64 *cg, const char* name, long long v, int has_init);
int  cg_arm64_decl_global_bool  (CodegenARM64 *cg, const char* name, int v, int has_init);
int  cg_arm64_decl_global_char  (CodegenARM64 *cg, const char* name, int v, int has_init);
int  cg_arm64_decl_global_float (CodegenARM64 *cg, const char* name, float v, int has_init);
int  cg_arm64_decl_global_double(CodegenARM64 *cg, const char* name, double v, int has_init);
int  cg_arm64_decl_global_string(CodegenARM64 *cg, const char* name, const char* s, int has_init);

// volcar la .data al final
void cg_arm64_flush_data(CodegenARM64 *cg);

void cg_arm64_store_int_imm(CodegenARM64* cg, const char* label, int value);
void cg_arm64_store_bool_imm   (struct CodegenARM64* cg, const char* label, int value01);
void cg_arm64_store_char_imm   (struct CodegenARM64* cg, const char* label, int ch /*0..255*/);
void cg_arm64_store_float_imm  (struct CodegenARM64* cg, const char* label, float fval);
void cg_arm64_store_double_imm (struct CodegenARM64* cg, const char* label, double dval);
void cg_arm64_store_string_lit (struct CodegenARM64* cg, const char* label, const char* s);


void cg_arm64_load_int(CodegenARM64* cg, const char* label, int dst_wreg);   
void cg_arm64_store_int(CodegenARM64* cg, int src_wreg, const char* label);
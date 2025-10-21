#include "arm_codegen_singleton.h"
#include "sys_emitter.h"
#include "codegen_arm64.h"
#ifndef MY_NULL
#define MY_NULL ((void*)0)
#endif
static int started = 0;
static int closed  = 0;
static Emitter em;
static CodegenARM64 cg;

void arm_codegen_begin_if_needed(void){
    if (started) return;
    em.fd = -1;
    if (em_open(&em, "salida.s") != 0) return;   
    cg_arm64_begin(&cg, &em);                   
    started = 1;
}

void arm_codegen_emit_println_from_cstring(const char* s){
    arm_codegen_begin_if_needed();
    if (!started) return;
    int id = cg_arm64_intern_str(&cg, s ? s : "");
    cg_arm64_write_literal_id(&cg, id);
    int idNL = cg_arm64_intern_str(&cg, "\n");
    cg_arm64_write_literal_id(&cg, idNL);
}


struct CodegenARM64* arm_codegen_handle(void){
    arm_codegen_begin_if_needed();
    return started ? &cg : (struct CodegenARM64*)0;
}
void arm_codegen_finalize(void){
    if (!started || closed) return;
    cg_arm64_emit_exit(&cg, 0);
    cg_arm64_flush_data(&cg); 
    cg_arm64_flush_rodata(&cg);
    em_close(&em);
    closed = 1;
}

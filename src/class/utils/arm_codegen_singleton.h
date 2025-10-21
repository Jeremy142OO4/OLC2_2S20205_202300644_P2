#pragma once


void arm_codegen_begin_if_needed(void);                
void arm_codegen_emit_println_from_cstring(const char* s);
void arm_codegen_finalize(void);                       

struct CodegenARM64;                     
struct CodegenARM64* arm_codegen_handle(void);
// src/asm/asm_decl.c
#include "asm_decl.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Entry {
    char* id;
    char* label;
    Tipo  tipo;
    struct Entry* next;
} Entry;

#define HB 257
static Entry* HT[HB];
static int g_seq = 0;

static unsigned h(const char* s){
    unsigned v=0; while(*s) v = (v<<5) + (unsigned char)*s++; return v % HB;
}

void asm_decl_init(void){ for(int i=0;i<HB;i++) HT[i]=NULL; }
void asm_decl_close(void){
    for(int i=0;i<HB;i++){
        Entry* e=HT[i];
        while(e){ Entry* n=e->next; free(e->id); free(e->label); free(e); e=n; }
        HT[i]=NULL;
    }
}

/* helpers de emisión */
static void emit_word(const char* lbl, unsigned v){
    asm_datos();
    asm_fmt("%s:\n", lbl);
    asm_ln("    .p2align 2");
    asm_fmt("    .word %u\n", v);
}
static void emit_quad(const char* lbl, unsigned long long v){
    asm_datos();
    asm_fmt("%s:\n", lbl);
    asm_ln("    .p2align 3");
    asm_fmt("    .quad %llu\n", (unsigned long long)v);
}
static void emit_byte(const char* lbl, unsigned v){
    asm_datos();
    asm_fmt("%s:\n", lbl);
    asm_fmt("    .byte %u\n", v & 0xFFu);
}

/* string: literal asciz + var .quad puntero al literal */
static void emit_string_ptr(const char* lbl, const char* s){
    char lit[256];
    snprintf(lit, sizeof(lit), "%s__lit", lbl);
    // literal (usa tu helper para escapar)
    asm_ro();
    asm_fmt("%s:        .ascii ", lit);
    // reutilizamos el escapador de asm_decl_str:
    asm_decl_str(lit, s ? s : "");
    // añadir terminador
    asm_ln("    .byte 0");
    // var = puntero
    asm_datos();
    asm_fmt("%s:\n", lbl);
    asm_ln("    .p2align 3");
    asm_fmt("    .quad %s\n", lit);
}

const char* asm_label_of(const char* id){
    if(!id) return NULL;
    unsigned idx = h(id);
    for(Entry* e=HT[idx]; e; e=e->next) if(strcmp(e->id,id)==0) return e->label;
    return NULL;
}

const char* asm_decl_global(const char* id, Tipo tipo, void* valor_inicial){
    if(!id) return NULL;

    /* si ya existe, no dupliques */
    const char* ya = asm_label_of(id);
    if (ya) return ya;

    /* crea entrada */
    unsigned idx = h(id);
    Entry* e = (Entry*)malloc(sizeof(Entry));
    e->id = strdup(id);
    char tmp[256]; snprintf(tmp, sizeof(tmp), "%s__g%d", id, g_seq++);
    e->label = strdup(tmp);
    e->tipo = tipo;
    e->next = HT[idx];
    HT[idx] = e;

    /* emitir reserva/valor */
    switch(tipo){
        case TIPO_ENTERO: {
            unsigned v = 0;
            if (valor_inicial) v = (unsigned)(*(int*)valor_inicial);
            emit_word(e->label, v);
        } break;
        case TIPO_BOOLEANO: {
            unsigned v = 0;
            if (valor_inicial) v = (*(int*)valor_inicial) ? 1u : 0u;
            emit_word(e->label, v);
        } break;
        case TIPO_CHAR: {
            unsigned v = 0;
            if (valor_inicial) v = (unsigned)(*(unsigned char*)valor_inicial);
            emit_byte(e->label, v);
        } break;
        case TIPO_DECIMAL: { // float → bits crudos en .word
            unsigned u = 0;
            if (valor_inicial){
                float f = *(float*)valor_inicial;
                memcpy(&u, &f, 4);
            }
            emit_word(e->label, u);
        } break;
        case TIPO_DOUBLE: { // double → bits crudos en .quad
            unsigned long long u = 0;
            if (valor_inicial){
                double d = *(double*)valor_inicial;
                memcpy(&u, &d, 8);
            }
            emit_quad(e->label, u);
        } break;
        case TIPO_CADENA: {
            const char* s = valor_inicial ? (const char*)valor_inicial : "";
            emit_string_ptr(e->label, s);
        } break;
        case TIPO_NULO:
        default:
            emit_word(e->label, 0);
            break;
    }

    return e->label;
}

#include "../ast/ast.h"
#include "../utils/asm_gen.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

static int gen_eval(const struct ASTNode *n);

/* --- utilidades para tipos --- */
static int es_bool(const char *s){
    return s && (!strcmp(s,"true") || !strcmp(s,"false"));
}
static int es_str(const char *s){
    size_t L = s ? strlen(s) : 0;
    return (L >= 2 && s[0] == '"' && s[L-1] == '"');
}
static int es_int(const char *s){
    if (!s || !*s) return 0;
    const char *p = s;
    if (*p == '+' || *p == '-') p++;
    if (!*p) return 0;
    for (; *p; p++) if (!isdigit((unsigned char)*p)) return 0;
    return 1;
}

/* --- quitar comillas a cadenas --- */
static void quitar_comillas(char *dst, size_t dstsz, const char *src){
    size_t L = strlen(src);
    size_t i = 1, j = 0, end = (L ? L-1 : 0);
    for (; i < end && j + 1 < dstsz; i++){
        char c = src[i];
        if (c == '\\' && i + 1 < end){
            char d = src[++i];
            if (d == 'n') c = '\n';
            else if (d == 't') c = '\t';
            else c = d;
        }
        dst[j++] = c;
    }
    dst[j] = 0;
}

/* --- evaluar literal --- */
static int gen_eval_lit(const char *v){
    if (es_bool(v)){
        asm_fmt("    mov     w2, #%d\n", !strcmp(v, "true") ? 1 : 0);
        return 1;
    }
    if (es_str(v)){
        char buf[1024];
        quitar_comillas(buf, sizeof(buf), v);
        asm_print_cstr_inline(buf);
        return 2;
    }
    if (es_int(v)){
        long long x = strtoll(v, NULL, 10);
        asm_cargar_x_imm("x2", (unsigned long long)x);
        return 0;
    }
    asm_ln("    mov     w2, #0");
    return 0;
}

/* --- evaluar binop --- */
static int gen_eval_bin(const struct ASTNode *n){
    int tl = gen_eval(n->left);
    if (tl == 2) tl = 0;
    asm_ln("    mov     w9, w2");
    int tr = gen_eval(n->right);
    if (tr == 2) tr = 0;

    const char *op = n->value ? n->value : "";
    if (!strcmp(op, "+"))  { asm_ln("    add w2, w9, w2"); return 0; }
    if (!strcmp(op, "-"))  { asm_ln("    sub w2, w9, w2"); return 0; }
    if (!strcmp(op, "*"))  { asm_ln("    mul w2, w9, w2"); return 0; }
    if (!strcmp(op, "/"))  { asm_ln("    sdiv w2, w9, w2"); return 0; }
    if (!strcmp(op, "%"))  {
        asm_ln("    sdiv w10, w9, w2");
        asm_ln("    msub w2, w10, w2, w9");
        return 0;
    }

    asm_ln("    cmp w9, w2");
    if (!strcmp(op, ">"))  { asm_ln("    cset w2, gt"); return 1; }
    if (!strcmp(op, ">=")) { asm_ln("    cset w2, ge"); return 1; }
    if (!strcmp(op, "<"))  { asm_ln("    cset w2, lt"); return 1; }
    if (!strcmp(op, "<=")) { asm_ln("    cset w2, le"); return 1; }
    if (!strcmp(op, "==")) { asm_ln("    cset w2, eq"); return 1; }
    if (!strcmp(op, "!=")) { asm_ln("    cset w2, ne"); return 1; }

    asm_ln("    mov w2, #0");
    return 0;
}

/* --- imprimir booleano --- */
static void gen_print_bool(void){
    asm_decl_str("str_true",  "true");
    asm_decl_str("str_false", "false");
    asm_ln("    cbz w2, 1f");
    asm_print_inline_lbl("str_true", 4);
    asm_ln("    b 2f");
    asm_ln("1:");
    asm_print_inline_lbl("str_false", 5);
    asm_ln("2:");
}

/* --- evaluar nodo genérico --- */
static int gen_eval(const struct ASTNode *n){
    if (!n){ asm_ln("    mov w2, #0"); return 0; }
    if (!strcmp(n->kind, "literal")) return gen_eval_lit(n->value);
    if (!strcmp(n->kind, "binop"))   return gen_eval_bin(n);
    asm_ln("    mov w2, #0");
    return 0;
}

/* --- función pública --- */
void gen_imprimir_expr(const struct ASTNode *n){
    asm_prog_inicio();
    asm_texto(); 
    int t = gen_eval(n);
    if (t == 2) { asm_print_cstr_inline("\n"); return; }
    if (t == 1) { gen_print_bool(); asm_print_cstr_inline("\n"); return; }
    asm_imprimir_entero_inline_desde_x2();
    asm_print_cstr_inline("\n");
}

#ifndef GEN_EXPR_H
#define GEN_EXPR_H

#include "../utils/asm_gen.h"

struct ASTNode;
typedef enum {
    N_INT, N_BOOL, N_STR,
    N_BIN,
    N_ID     
} NodoTag;

void gen_imprimir_expr(const struct ASTNode *n);

#endif
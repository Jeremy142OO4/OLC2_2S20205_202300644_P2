#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct ASTNode* make_node(const char* kind, const char* value, struct ASTNode* left, struct ASTNode* right) {
    struct ASTNode* n = malloc(sizeof(struct ASTNode));
    n->kind = strdup(kind);
    n->value = value ? strdup(value) : NULL;
    n->left = left;
    n->right = right;
    return n;
}

struct ASTNode* ast_link(struct ASTNode* left, struct ASTNode* right) { return make_node("link", NULL, left, right); }
struct ASTNode* ast_print_stmt(struct ASTNode* expr) { return make_node("print", NULL, expr, NULL); }
struct ASTNode* ast_var_decl(char* id, struct ASTNode* type, struct ASTNode* expr) { return make_node("var", id, type, expr); }
struct ASTNode* ast_var_decl_const(char* id, struct ASTNode* type, struct ASTNode* expr) { return make_node("var_const", id, type, expr); }
struct ASTNode* ast_assign(char* op, struct ASTNode* expr, struct ASTNode* id) { return make_node("assign", op, id, expr); }
struct ASTNode* ast_type(char* typeName) { return make_node("type", typeName, NULL, NULL); }
struct ASTNode* ast_while(struct ASTNode* cond,struct ASTNode* body) { return make_node("while", NULL, cond, body); }
struct ASTNode* ast_incremento_decremento(char* op, struct ASTNode* expr) { return make_node("incremento_decremento", op, expr, NULL); }
struct ASTNode* ast_switch(struct ASTNode* expr, struct ASTNode* cases) {return make_node("switch", NULL, expr, cases);}
struct ASTNode* ast_case(struct ASTNode* value, struct ASTNode* body) {return make_node("case", NULL, value, body);}
struct ASTNode* ast_if(struct ASTNode* cond,struct ASTNode* then_body,struct ASTNode* else_part) {return make_node("if", NULL, cond, ast_link(then_body, else_part));}
struct ASTNode* ast_literal(char* value) { return make_node("literal", value, NULL, NULL); }
struct ASTNode* ast_break() { return make_node("break", NULL, NULL, NULL); }
struct ASTNode* ast_continue() { return make_node("continue", NULL, NULL, NULL); }
struct ASTNode* ast_join(struct ASTNode* delimitador,struct ASTNode* elemetos) { return make_node("join", NULL, delimitador, elemetos); }
struct ASTNode* ast_parser(char* func, struct ASTNode* expr) { return make_node("parser", func, expr, NULL); }
struct ASTNode* ast_return(struct ASTNode* expr) { return make_node("return", NULL, expr, NULL); }
struct ASTNode* ast_vector_decl(char* id, struct ASTNode* tipo, struct ASTNode* tam) { return make_node("vector_decl", id, ast_link(tipo,tam), NULL); }
struct ASTNode* ast_vector_decl_init(char* id, struct ASTNode* tipo, struct ASTNode* valores) { return make_node("vector_decl_init", id, tipo, valores); }
struct ASTNode* ast_parametros(char* id, struct ASTNode* tipo, struct ASTNode* siguiente) { return make_node("parametro", id, tipo, siguiente); }
struct ASTNode* ast_for(struct ASTNode* init, struct ASTNode* cond, struct ASTNode* cambio, struct ASTNode* body) { return make_node("for", NULL, ast_link(init, cond), ast_link(cambio, body)); }
struct ASTNode* ast_cast(struct ASTNode* value,struct ASTNode* type) { return make_node("cast", NULL, value, type); }
struct ASTNode* ast_identifier(char* name) { return make_node("id", name, NULL, NULL); }
struct ASTNode* ast_funcion_call(char* nombre, struct ASTNode* parametros) { return make_node("llamada_funcion", nombre, parametros, NULL); }
struct ASTNode* ast_funcion_decl(char* nombre, struct ASTNode* parametros, struct ASTNode* tipo, struct ASTNode* instrucciones) { return make_node("funcion_decl", nombre, parametros, ast_link(tipo, instrucciones)); }
struct ASTNode* ast_binop(char* op, struct ASTNode* left, struct ASTNode* right) { return make_node("binop", op, left, right); }
struct ASTNode* ast_index1(char* id, struct ASTNode* idx) { return make_node("index1", id, ast_identifier(id), idx); }
struct ASTNode* ast_array_length(char* id) {return make_node("array_length", id, NULL, NULL);}
struct ASTNode* ast_indexof(struct ASTNode* valor, struct ASTNode* clave) {return make_node("array_indexof",NULL,valor,clave);}
struct ASTNode* ast_array_add(char* id, struct ASTNode* valor) {return make_node("array_add", id, ast_identifier(id), valor);}
struct ASTNode* ast_unop(char* op, struct ASTNode* expr) { return make_node("unop", op, expr, NULL); }

void ast_print(struct ASTNode* node, int depth) {
    if (!node) return;
    for (int i=0; i<depth; i++) printf("  ");
    printf("%s", node->kind);
    if (node->value) printf(" (%s)", node->value);
    printf("\n");
    ast_print(node->left, depth+1);
    ast_print(node->right, depth+1);
}

static void escape_dot_label(const char* src, char* dest, size_t max) {
    if (!src) { dest[0] = '\0'; return; }
    size_t j = 0;
    for (size_t i = 0; src[i] && j < max - 2; i++) {
        if (src[i] == '"' || src[i] == '\\') {
            if (j < max - 2) dest[j++] = '\\';
        }
        dest[j++] = src[i];
    }
    dest[j] = '\0';
}

static void ast_graphviz_rec(FILE* f, struct ASTNode* node, int* id_counter) {
    if (!node) return;

    int my_id = (*id_counter)++;

    const char* color = "snow";

    // Escribir el nodo con label seguro
    fprintf(f, "  node%d [label=\"%s", my_id, node->kind ? node->kind : "null");
    if (node->value) {
        char safe[512];
        escape_dot_label(node->value, safe, sizeof(safe));
        fprintf(f, "\\n%s", safe);
    }
    fprintf(f, "\", shape=ellipse, style=filled, fillcolor=%s, color=black, fontcolor=black, fontname=\"Arial\"];\n", color);

    // Guardar IDs de hijos antes de recursión
    if (node->left) {
        int left_id = *id_counter;
        ast_graphviz_rec(f, node->left, id_counter);
        fprintf(f, "  node%d -> node%d;\n", my_id, left_id);
    }
    if (node->right) {
        int right_id = *id_counter;
        ast_graphviz_rec(f, node->right, id_counter);
        fprintf(f, "  node%d -> node%d;\n", my_id, right_id);
    }
}

void ast_print_graphviz(struct ASTNode* root, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) {
        perror("fopen");
        return;
    }

    fprintf(f, "digraph AST {\n");
    fprintf(f, "  rankdir=TB;\n");
    fprintf(f, "  nodesep=0.6;\n");
    fprintf(f, "  ranksep=0.8;\n");
    fprintf(f, "  node [shape=ellipse, style=filled, fillcolor=snow, fontcolor=black, color=black, fontname=\"Arial\"];\n");
    fprintf(f, "  edge [color=black];\n");

    int id_counter = 0;
    ast_graphviz_rec(f, root, &id_counter);

    fprintf(f, "}\n");
    fclose(f);
}
#include <stdio.h>
#ifndef AST_H
#define AST_H

struct ASTNode {
    char* kind;
    char* value;
    struct ASTNode* left;
    struct ASTNode* right;
};

struct ASTNode* ast_link(struct ASTNode* left, struct ASTNode* right);
struct ASTNode* ast_print_stmt(struct ASTNode* expr);
struct ASTNode* ast_var_decl(char* id, struct ASTNode* type, struct ASTNode* expr);
struct ASTNode* ast_var_decl_const(char* id, struct ASTNode* type, struct ASTNode* expr);
struct ASTNode* ast_assign(char* op, struct ASTNode* expr, struct ASTNode* id);
struct ASTNode* ast_type(char* typeName);
struct ASTNode* ast_incremento_decremento(char* op, struct ASTNode* expr);
struct ASTNode* ast_switch(struct ASTNode* expr, struct ASTNode* cases);
struct ASTNode* ast_case(struct ASTNode* value, struct ASTNode* body);
struct ASTNode* ast_if(struct ASTNode* cond,struct ASTNode* then_body, struct ASTNode* else_part);
struct ASTNode* ast_for(struct ASTNode* init, struct ASTNode* cond, struct ASTNode* cambio, struct ASTNode* body);    
struct ASTNode* ast_while(struct ASTNode* cond,struct ASTNode* body);
struct ASTNode* ast_break();
struct ASTNode* ast_continue();
struct ASTNode* ast_parser(char* func, struct ASTNode* expr);
struct ASTNode* ast_join(struct ASTNode* delimitador,struct ASTNode* elemetos);
struct ASTNode* ast_return(struct ASTNode* expr);
struct ASTNode* ast_parametros(char* id, struct ASTNode* tipo, struct ASTNode* siguiente);
struct ASTNode* ast_funcion_decl(char* nombre, struct ASTNode* parametros, struct ASTNode* tipo, struct ASTNode* instrucciones);
struct ASTNode* ast_literal(char* value);
struct ASTNode* ast_cast(struct ASTNode* value, struct ASTNode* type);
struct ASTNode* ast_identifier(char* name);
struct ASTNode* ast_funcion_call(char* nombre, struct ASTNode* parametros);
struct ASTNode* ast_binop(char* op, struct ASTNode* left, struct ASTNode* right);
struct ASTNode* ast_unop(char* op, struct ASTNode* expr);
struct ASTNode* ast_vector_decl(char* id, struct ASTNode* tipo, struct ASTNode* tam);
struct ASTNode* ast_index1(char* id, struct ASTNode* idx);
struct ASTNode* ast_vector_decl_init(char* id, struct ASTNode* tipo, struct ASTNode* valores);
struct ASTNode* ast_array_length(char* id);
struct ASTNode* ast_indexof(struct ASTNode* valor, struct ASTNode* clave);
struct ASTNode* ast_array_add(char* id, struct ASTNode* valor);


void ast_print(struct ASTNode* node, int depth);
void ast_print_graphviz(struct ASTNode* root, const char* filename);
static void ast_graphviz_rec(FILE* f, struct ASTNode* node, int* id_counter);
#endif
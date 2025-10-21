#include <stdio.h>
#include <stdlib.h>
#include "./ast/ast.h"
#include "./class/expresiones/ejecutar.h"
#include "./class/utils/errores.h"
#include "./class/utils/arm_codegen_singleton.h"

// Declaraciones externas que Flex/Bison generan
extern int yyparse(void);
extern FILE *yyin;
extern struct ASTNode* root;

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    yyin = fopen(argv[1], "r");
    if (!yyin) return 1;

    int resultado = yyparse();

    if (resultado == 0) {
        if (errores_init("tabla_errores.txt") != 0) return 1;

        struct entorno* global = nuevo_entorno(NULL);
        TipoRetorno res = ejecutar(root, global);

        arm_codegen_finalize();

        errores_close();
        exportar_tabla(global,"tabla_simbolos.txt","global");
        ast_print_graphviz(root, "ast.dot");
        if (system("dot -Tsvg ast.dot -o ast.svg") != 0) {
            printf("Error al generar el ast.svg\n");
        } else {
            printf("AST generado con exito: ast.svg\n");
        }
    } else {
        printf("Error en el parseo.\n");
    }

    fclose(yyin);
    return resultado;
}

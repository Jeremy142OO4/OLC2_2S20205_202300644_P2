#include <stdio.h>
#include <stdlib.h>
#include "./ast/ast.h"
#include "./class/expresiones/ejecutar.h"
#include "./class/utils/errores.h"
// Declaraciones externas que Flex/Bison generan
extern int yyparse(void);
extern FILE *yyin;
extern struct ASTNode* root;

int main(int argc, char *argv[]) {
    // Verificar argumento
    if (argc < 2) {
        //fprintf(stderr, "Uso: %s <archivo>\n", argv[0]);
        return 1;
    }

    // Abrir el archivo de entrada
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        //perror("No se pudo abrir el archivo");
        return 1;
    }

    //printf("Iniciando el parser con archivo: %s\n", argv[1]);

    // Llamar al parser
    int resultado = yyparse();

    if (resultado == 0) {
        //printf("Parseo completado exitosamente.\n");
        //ast_print(root, 0);
        if (errores_init("tabla_errores.txt") != 0) {
        fprintf(stderr, "No se pudo abrir tabla_errores.txt\n");
        }
        struct entorno* global = nuevo_entorno(NULL);
        TipoRetorno res = ejecutar(root, global);
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
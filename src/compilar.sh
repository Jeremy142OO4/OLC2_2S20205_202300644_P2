#!/bin/bash

# Compilar lexer con Flex
flex -l -o ./lenguaje/lex.yy.c ./lenguaje/lexer.l

# Compilar parser con Bison
bison -dv -o ./lenguaje/parser.tab.c ./lenguaje/parser.y

# Compilar todo con GCC
gcc -o compilador \
    main.c \
    lenguaje/parser.tab.c \
    lenguaje/lex.yy.c \
    ast/ast.c \
    class/utils/errores.c \
    class/utils/sys_emitter.c \
    class/utils/codegen_arm64.c \
    class/utils/num_to_str.c \
    class/utils/arm_codegen_singleton.c \
    class/entorno/entorno.c \
    class/expresiones/ejecutar.c \
    class/expresiones/aritmetico.c \
    class/expresiones/literal.c \
    class/expresiones/relacional.c \
    class/expresiones/id.c \
    class/expresiones/casteo.c \
    class/expresiones/logico.c \
    class/expresiones/parser.c \
    class/expresiones/join.c \
    class/expresiones/indexOf.c \
    class/expresiones/vector_index.c \
    class/expresiones/llamada_funcion.c \
    class/expresiones/length.c  \
    class/instrucciones/imprimir.c \
    class/instrucciones/declarar_var.c \
    class/instrucciones/asignacion.c \
    class/instrucciones/incremento_decremento.c \
    class/instrucciones/if.c \
    class/instrucciones/switch.c \
    class/instrucciones/while.c \
    class/instrucciones/for.c \
    class/instrucciones/break.c \
    class/instrucciones/continuar.c \
    class/instrucciones/retornar.c \
    class/instrucciones/declarar_funcion.c \
    class/instrucciones/declarar_parametros.c \
    class/instrucciones/declarar_vector.c \
    class/instrucciones/array_add.c \
    -Iast -Ilenguaje -Iclass/expresiones -Iclass/instrucciones -Iclass/entorno\
    -lm
# Mensaje final
echo "✅ Compilación completada. Ejecuta ./compilador"

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "../ast/ast.h"
extern int yylex();
void yyerror(const char *s);
struct ASTNode* root = NULL;
%}




%locations


%union {
    char* str;

    struct ASTNode* node;
}




%token TK_PRINT TK_IF TK_ELSE TK_WHILE TK_FOR TK_DO TK_SWITCH TK_CASE TK_CONTINUE TK_BREAK TK_RETURN TK_VOID TK_MAIN TK_DEFAULT TK_NUEVO
%token TK_INT TK_FLOAT TK_STRING TK_BOOL TK_CHAR TK_PARSEINT TK_PARSEFLOAT TK_PARSEDOUBLE TK_VALUEOF TK_JOIN TK_DOUBLE TK_EQUALS TK_LENGTH
%token TK_FINAL  TK_INDEXOF TK_ADD


%token TK_PA TK_PC TK_LLA TK_LLC TK_PTCOMA TK_DOSPUNTOS TK_COMA
%token TK_CA TK_CC    


%token TK_MAYOR TK_MENOR
%token TK_MAYOR_IGUAL TK_MENOR_IGUAL


%token TK_AND TK_OR TK_NOT


%token TK_SUMA TK_RESTA
%token TK_MULTIPLICACION TK_DIVISION TK_MODULAR TK_AMPER TK_PIPE TK_POT TK_MAYOR_MAYOR TK_MENOR_MENOR


%token TK_IGUAL 
%token TK_MAS_IGUAL TK_MENOS_IGUAL
%token TK_POR_IGUAL TK_DIVIDIR_IGUAL TK_PORCENTAJE_IGUAL
%token TK_AND_IGUAL TK_OR_IGUAL
%token TK_POTENCIA_IGUAL
%token TK_MAYOR_MAYOR_IGUAL TK_MENOR_MENOR_IGUAL
%token TK_DOSPUNTOS_IGUAL
%token TK_IGUAL_IGUAL TK_DIFERENTE

%token <str> INT DECIMAL CARACTER CADENA ID BOOL DOUBLE

%type <node> TIPO_PARAM
%type <node> inicio listainstrucciones instruccion 
%type <node> TIPO expr ARITMETICOS RELACIONALES LOGICOS VALORES LLAMADA_FUNCION LLAMADA_PROCEDIMIENTO DECLARAR_VECTOR 
%type <node> DECLARACION ASIGNACION IMPRIMIR IF INCREMENTO_DECREMENTO SWITCH CASES CASE BREAK WHILE FOR CONTINUAR DECLARAR_FUNCION PARAMETROS RETORNAR
%type <str> OP_ASIGNACION 

%left TK_OR
%left TK_AND
%right TK_NOT
%right UMINUS
%left TK_MENOR TK_MENOR_IGUAL TK_MAYOR TK_MAYOR_IGUAL TK_IGUAL_IGUAL TK_DIFERENTE 
%left TK_SUMA TK_RESTA TK_AMPER TK_PIPE TK_POT TK_MAYOR_MAYOR TK_MENOR_MENOR
%left TK_MULTIPLICACION TK_DIVISION TK_MODULAR



%%

inicio:
    listainstrucciones { printf("Analisis completado.\n"); root = $1;}
    ;

listainstrucciones:
      listainstrucciones instruccion    {$$ = ast_link($1, $2);}
    | instruccion                       { $$ = $1; }
    ;

instruccion:
      IMPRIMIR 
    | DECLARACION
    | ASIGNACION 
    | IF
    | INCREMENTO_DECREMENTO TK_PTCOMA
    | SWITCH
    | BREAK
    | WHILE
    | FOR
    | CONTINUAR
    | RETORNAR
    | DECLARAR_FUNCION
    | LLAMADA_PROCEDIMIENTO
    | DECLARAR_VECTOR
    | ID TK_ADD TK_PA expr TK_PC TK_PTCOMA   { $$ = ast_array_add($1, $4); }
    ;

DECLARACION:
      TIPO ID TK_IGUAL expr TK_PTCOMA            { $$ = ast_var_decl($2, $1, $4); }
    | TIPO ID  TK_PTCOMA                         { $$ = ast_var_decl($2, $1, NULL); }
    | TK_FINAL TIPO ID TK_IGUAL expr  TK_PTCOMA  {  $$ = ast_var_decl_const($3, $2, $5); }
    ;

DECLARAR_VECTOR:
    TIPO TK_CA TK_CC ID TK_IGUAL TK_NUEVO TIPO TK_CA expr TK_CC TK_PTCOMA
      { if ($1 && $7 && strcmp($1->value,$7->value)==0) $$ = ast_vector_decl($4,$1,$9);
      else $$ = NULL; }
    |
    TIPO TK_CA TK_CC ID TK_IGUAL TK_LLA VALORES TK_LLC TK_PTCOMA { $$ = ast_vector_decl_init($4,$1,$7); }
    ;

ASIGNACION:
      expr OP_ASIGNACION expr TK_PTCOMA                  { $$ = ast_assign($2, $3, $1); }
    | ID TK_CA expr TK_CC OP_ASIGNACION expr TK_PTCOMA   { $$ = ast_assign($5, $6, ast_index1($1, $3)); }
    ;

IMPRIMIR:
      TK_PRINT TK_PA expr TK_PC  TK_PTCOMA        { $$ = ast_print_stmt($3);}

INCREMENTO_DECREMENTO:
      expr TK_SUMA TK_SUMA         { $$ = ast_incremento_decremento("++", $1); }
    | expr TK_RESTA TK_RESTA       { $$ = ast_incremento_decremento("--", $1); }
    ;

IF: 
    TK_IF TK_PA expr TK_PC TK_LLA listainstrucciones TK_LLC { $$ = ast_if($3, $6, NULL); }
  | TK_IF TK_PA expr TK_PC TK_LLA listainstrucciones TK_LLC TK_ELSE TK_LLA listainstrucciones TK_LLC { $$ = ast_if($3, $6, $10); }
  | TK_IF TK_PA expr TK_PC TK_LLA listainstrucciones TK_LLC TK_ELSE IF { $$ = ast_if($3, $6, $9); } 
  ;

BREAK:
    TK_BREAK TK_PTCOMA      { $$ = ast_break(); }
  ;

CONTINUAR:
    TK_CONTINUE TK_PTCOMA   { $$ = ast_continue(); }

RETORNAR:
    TK_RETURN expr TK_PTCOMA { $$ = ast_return($2); }
  | TK_RETURN TK_PTCOMA      { $$ = ast_return(NULL);}
  ;

SWITCH: 
    TK_SWITCH TK_PA expr TK_PC TK_LLA CASES TK_LLC { $$ = ast_switch($3, $6); }
  ;

CASES: 
    CASE CASES              { $$ = ast_link($1, $2); }
  | CASE                    { $$ = $1; }
  ;

CASE: 
    TK_CASE expr TK_DOSPUNTOS listainstrucciones  { $$ = ast_case($2, $4); }
  | TK_DEFAULT TK_DOSPUNTOS listainstrucciones    { $$ = ast_case(NULL, $3); }   
  ;

WHILE:
    TK_WHILE TK_PA expr TK_PC TK_LLA listainstrucciones TK_LLC { $$ = ast_while($3, $6); }
  ;

FOR:
    TK_FOR TK_PA DECLARACION expr TK_PTCOMA INCREMENTO_DECREMENTO TK_PC TK_LLA listainstrucciones TK_LLC { $$ = ast_for($3, $4, $6, $9); }
  ;

TIPO_PARAM:
      TIPO                        { $$ = $1; }
    | TIPO TK_CA TK_CC            {char *t = (char*)malloc(strlen($1->value) + 3);sprintf(t, "%s[]", $1->value);$$ = ast_type(t);    }
    ;

PARAMETROS:
      TIPO_PARAM ID TK_COMA PARAMETROS   { $$ = ast_parametros($2, $1, $4); }
    | TIPO_PARAM ID                      { $$ = ast_parametros($2, $1, NULL); }
    |                              { $$ = NULL; }
    ;

DECLARAR_FUNCION:
      TIPO ID TK_PA PARAMETROS TK_PC TK_LLA listainstrucciones TK_LLC     { $$ = ast_funcion_decl($2, $4, $1, $7); }
    | TK_VOID ID TK_PA PARAMETROS TK_PC TK_LLA listainstrucciones TK_LLC  { $$ = ast_funcion_decl($2, $4, NULL, $7); }
    ;

LLAMADA_PROCEDIMIENTO:
    ID TK_PA VALORES TK_PC TK_PTCOMA { $$ = ast_funcion_call($1, $3); }
    ;



TIPO:
      TK_INT     { $$ = ast_type("int");}
    | TK_STRING  { $$ = ast_type("string");}
    | TK_FLOAT   { $$ = ast_type("float");}
    | TK_BOOL    { $$ = ast_type("bool");}
    | TK_CHAR    { $$ = ast_type("char");}
    | TK_DOUBLE  { $$ = ast_type("double");}
    ;

expr:
     TK_PA expr TK_PC          {$$ = $2;}
    | TK_PARSEINT TK_PA expr TK_PC    { $$ = ast_parser("parseInt", $3 );}
    | TK_PARSEFLOAT TK_PA expr TK_PC  { $$ = ast_parser("parseFloat", $3); }
    | TK_PARSEDOUBLE TK_PA expr TK_PC { $$ = ast_parser("parseDouble", $3); }
    | TK_VALUEOF TK_PA expr TK_PC     { $$ = ast_parser("valueOf", $3); }
    | TK_JOIN TK_PA expr TK_COMA VALORES TK_PC { $$ = ast_join($3, $5); }
    | TK_PA TIPO TK_PC expr     { $$ = ast_cast($4, $2); }
    | TK_INDEXOF TK_PA expr  TK_COMA expr TK_PC { $$ = ast_indexof($3,$5);}
    | TK_RESTA expr %prec UMINUS { $$ = ast_unop("-", $2); }
    | INT                       { $$ = ast_literal($1); }
    | DECIMAL                   { $$ = ast_literal($1); }
    | CARACTER                  { $$ = ast_literal($1); }
    | CADENA                    { $$ = ast_literal($1); }
    | BOOL                      { $$ = ast_literal($1); }
    | DOUBLE                    { $$ = ast_literal($1); }
    | ID                        { $$ = ast_identifier($1); }
    | ARITMETICOS               { $$ = $1; }
    | RELACIONALES              { $$ = $1; }
    | LOGICOS                   { $$ = $1; }
    | LLAMADA_FUNCION           { $$ = $1; }
    | ID TK_CA expr TK_CC       { $$ = ast_index1($1, $3); }
    | ID TK_LENGTH              { $$ = ast_array_length($1); }
    
    ;



ARITMETICOS:
      expr TK_SUMA expr             { $$ = ast_binop("+", $1, $3); }
    | expr TK_RESTA expr            { $$ = ast_binop("-", $1, $3); }
    | expr TK_MULTIPLICACION expr   { $$ = ast_binop("*", $1, $3); }
    | expr TK_DIVISION expr         { $$ = ast_binop("/", $1, $3); }
    | expr TK_MODULAR expr          { $$ = ast_binop("%", $1, $3); }
    | expr TK_AMPER expr            { $$ = ast_binop("&", $1, $3); }
    | expr TK_PIPE expr             { $$ = ast_binop("|", $1, $3); }
    | expr TK_POT expr              { $$ = ast_binop("^", $1, $3); }
    | expr TK_MAYOR_MAYOR expr      { $$ = ast_binop(">>", $1, $3); }
    | expr TK_MENOR_MENOR expr      { $$ = ast_binop("<<", $1, $3); }
    ;

OP_ASIGNACION:
      TK_IGUAL               { $$ = strdup("="); }
    | TK_MAS_IGUAL           { $$ = strdup("+="); }
    | TK_MENOS_IGUAL         { $$ = strdup("-="); }
    | TK_POR_IGUAL           { $$ = strdup("*="); }
    | TK_DIVIDIR_IGUAL       { $$ = strdup("/="); }
    | TK_PORCENTAJE_IGUAL    { $$ = strdup("%="); }
    | TK_AND_IGUAL           { $$ = strdup("&="); }
    | TK_OR_IGUAL            { $$ = strdup("|="); }
    | TK_POTENCIA_IGUAL      { $$ = strdup("^="); }   
    | TK_MAYOR_MAYOR_IGUAL   { $$ = strdup(">>="); }
    | TK_MENOR_MENOR_IGUAL   { $$ = strdup("<<="); }
    | TK_DOSPUNTOS_IGUAL     { $$ = strdup(":="); }
    ;


RELACIONALES:
      expr TK_IGUAL_IGUAL expr      { $$ = ast_binop("==", $1, $3); }
    | expr TK_EQUALS TK_PA expr TK_PC { $$ = ast_binop("==", $1, $4); }
    | expr TK_DIFERENTE expr        { $$ = ast_binop("!=", $1, $3); }
    | expr TK_MAYOR_IGUAL expr      { $$ = ast_binop(">=", $1, $3); }
    | expr TK_MENOR_IGUAL expr      { $$ = ast_binop("<=", $1, $3); }
    | expr TK_MAYOR expr            { $$ = ast_binop(">", $1, $3); }
    | expr TK_MENOR expr            { $$ = ast_binop("<", $1, $3); }
    ;


LOGICOS:
      expr TK_AND expr              { $$ = ast_binop("&&", $1, $3); }
    | expr TK_OR expr               { $$ = ast_binop("||", $1, $3); }
    | TK_NOT expr                   { $$ = ast_unop("!", $2); }
    ;


VALORES:
      expr TK_COMA VALORES     { $$ = ast_link($1, $3); }
    | expr                     { $$ = ast_link($1, NULL); }
    |                         { $$ = NULL; }
    ;

LLAMADA_FUNCION:
    ID TK_PA VALORES TK_PC { $$ = ast_funcion_call($1, $3); }
    ;

%%






void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
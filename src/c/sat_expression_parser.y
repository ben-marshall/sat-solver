
%defines
%verbose

%{

#include <stdio.h>
#define YYERROR_VERBOSE 1

%}

/* BISON Declarations */
%token TOK_DIGIT  
%token TOK_ID     
%token TOK_END    
%token TOK_ASSIGN 
%token TOK_OP_EQ  
%token TOK_OP_AND 
%token TOK_OP_OR  
%token TOK_OP_XOR 
%token TOK_OP_NOT 
%token TOK_OB     
%token TOK_CB     

%left TOK_OP_AND TOK_OP_OR TOK_OP_XOR TOK_OP_EQ
%left TOK_NOT

/* Grammar follows */
%%
input:    /* empty string */
| input assignment
| TOK_END
;

assignment : TOK_ID TOK_ASSIGN expression
;

expression :
    expression_unary
|   expression_binary
;

expression_binary : 
    TOK_ID TOK_OP_AND TOK_ID
|   TOK_ID TOK_OP_OR  TOK_ID
|   TOK_ID TOK_OP_XOR TOK_ID
|   TOK_ID TOK_OP_EQ  TOK_ID
;

expression_unary :
    TOK_OP_NOT  TOK_ID
;
%%

yyerror (char *s)
{
      printf ("[PARSE ERROR] %s\n", s);
}

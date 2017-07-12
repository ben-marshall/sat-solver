
%defines
%verbose

%{

#define YYERROR_VERBOSE 1

#include <stdio.h>
#include <math.h>

#include "../sat-expression.h"

//! @brief A linked list of expressions from the parsed file.
sat_binary_expression * yy_expressions;


/*!
@brief Adds a new binary expression to the linked list yy_expressions
*/
void yy_add_expression(sat_binary_expression * toadd)
{
    if(yy_expressions == NULL) {
        yy_expressions = toadd;
    } else {
        toadd ->next = yy_expressions;
        yy_expressions = toadd;
    }
}

%}

/* BISON Declarations */
%token TOK_DIGIT  
%token <vid> TOK_ID     
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

%union {
    char * vid;
    int    integer;
}

/* Grammar follows */
%%
input:    /* empty string */
| input assignment
| TOK_END
;

assignment : expression
;

expression :
    expression_unary
|   expression_binary
;

expression_binary : 
   TOK_ID TOK_ASSIGN TOK_ID TOK_OP_AND TOK_ID{
        sat_binary_expression * toadd = sat_new_binary_expression(
            atoi($<vid>1),atoi($<vid>3), atoi($<vid>5), OP_AND
        );
        yy_add_expression(toadd);
    }
|  TOK_ID TOK_ASSIGN TOK_ID TOK_OP_OR  TOK_ID{
        sat_binary_expression * toadd = sat_new_binary_expression(
            atoi($<vid>1),atoi($<vid>2), atoi($<vid>5), OP_OR
        );
        
        yy_add_expression(toadd);
    }
|  TOK_ID TOK_ASSIGN TOK_ID TOK_OP_XOR TOK_ID{
        sat_binary_expression * toadd = sat_new_binary_expression(
            atoi($<vid>1),atoi($<vid>3), atoi($<vid>5), OP_XOR
        );
        yy_add_expression(toadd);
        
    }
|  TOK_ID TOK_ASSIGN TOK_ID TOK_OP_EQ  TOK_ID{
        sat_binary_expression * toadd = sat_new_binary_expression(
            atoi($<vid>1),atoi($<vid>3), atoi($<vid>5), OP_EQ
        );
        yy_add_expression(toadd);
        
    }
;

expression_unary :
    TOK_ID TOK_ASSIGN TOK_OP_NOT  TOK_ID {
        sat_binary_expression * toadd = sat_new_binary_expression(
            atoi($<vid>1),atoi($<vid>4), atoi($<vid>4), OP_NOT
        );
        yy_add_expression(toadd);
        
    }
;
%%

yyerror (char *s)
{
      printf ("[PARSE ERROR] %s\n", s);
}

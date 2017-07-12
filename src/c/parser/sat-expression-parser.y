
%defines
%verbose

%{

#define YYERROR_VERBOSE 1

#include <stdio.h>
#include <math.h>

#include "../sat-expression.h"

void yy_add_expression(sat_expression_node * toadd);
int  yyerror          (const char          * s    );

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
    }
|  TOK_ID TOK_ASSIGN TOK_ID TOK_OP_OR  TOK_ID{
    }
|  TOK_ID TOK_ASSIGN TOK_ID TOK_OP_XOR TOK_ID{
        
    }
|  TOK_ID TOK_ASSIGN TOK_ID TOK_OP_EQ  TOK_ID{
        
    }
;

expression_unary :
    TOK_ID TOK_ASSIGN TOK_OP_NOT  TOK_ID {
        
    }
;
%%

int yyerror (const char *s)
{
      printf ("[PARSE ERROR] %s\n", s);
}


/*!
@brief Adds a new binary expression to the linked list yy_expressions
*/
void yy_add_expression(sat_expression_node * toadd)
{
}

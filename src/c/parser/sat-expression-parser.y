
%defines
%verbose

%{

#define YYERROR_VERBOSE 1

#include <stdio.h>
#include <math.h>

#include "sat-expression.h"

void yy_add_expression(sat_expression_node * toadd);
int  yyerror          (const char          * s    );

//! @note Declared in sat-expression.h
extern sat_assignment * yy_assignments;

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

%type <expr>    expression_unary
%type <expr>    expression_binary
%type <expr>    expression
%type <var>     variable
%type <assign>  assignment
%type <assign>  input
%type <assign>  start

%union {
    char * vid;
    int    integer;
    sat_expression_node     * expr;
    sat_expression_variable * var;
    sat_assignment          * assign;
}

/* Grammar follows */
%%

start : input TOK_END {
    $$ = $1;
    yy_assignments = $$;
}

input:    /* empty string */ {
    $$ = NULL;
}
| input assignment {
    $$ = $2;
    if ($1 != NULL)
    {
        $$ -> next = $1;
    }
}
;

assignment : variable TOK_ASSIGN expression {
    $$ = sat_new_assignment($1,$3);
}
;

expression :
    expression_unary {
    $$ = $1;
}
|   expression_binary {
    $$ = $1;
}
|   variable {
    $$ = sat_new_leaf_expression_node($1);
};

expression_binary : 
    expression TOK_OP_AND expression{
    $$ = sat_new_binary_expression_node($1,$3,SAT_OP_AND);
    }
|  expression TOK_OP_OR  expression{
    $$ = sat_new_binary_expression_node($1,$3,SAT_OP_OR );
    }
|  expression TOK_OP_XOR expression{
    $$ = sat_new_binary_expression_node($1,$3,SAT_OP_XOR);
    }
|  expression TOK_OP_EQ  expression{
    $$ = sat_new_binary_expression_node($1,$3,SAT_OP_EQ );
    }
;

expression_unary :
    TOK_OP_NOT expression {
    $$ = sat_new_unary_expression_node($2, SAT_OP_NOT); 
    }
;

variable : TOK_ID {
    $$ = sat_new_named_expression_variable($1);
};

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

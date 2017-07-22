
%defines
%verbose

%{

#define YYERROR_VERBOSE 1

#include <stdio.h>
#include <math.h>

#include "sat-expression.h"

int  yyerror          (const char          * s    );

//! @note Declared in sat-expression.h
extern sat_assignment * yy_assignments;

%}

/* BISON Declarations */
%token TOK_ZERO
%token TOK_ONE
%token <vid> TOK_ID     
%token TOK_END    
%token TOK_ASSIGN 
%token TOK_OP_NE  
%token TOK_OP_EQ  
%token TOK_OP_AND 
%token TOK_OP_OR  
%token TOK_OP_XOR 
%token TOK_OP_NOT 
%token TOK_OB     
%token TOK_CB     
%token TOK_OP     
%token TOK_CP     
%token TOK_EXPECT 
%token TOK_DOMAIN 

%left TOK_OP_OR
%left TOK_OP_AND 
%left TOK_OP_XOR
%right TOK_NOT

%type <expr>    expression_unary
%type <expr>    expression_binary
%type <expr>    expression
%type <var>     variable
%type <assign>  assignment
%type <assign>  input_assignments
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

start : input_assignments unary_constraints expectations TOK_END {
    $$ = $1;
    yy_assignments = $$;
}

input_assignments:    /* empty string */ {
    $$ = NULL;
}
| input_assignments assignment {
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
    TOK_OB expression_unary TOK_CB {
    $$ = $2;
    }
|   expression_unary {
    $$ = $1;
    }
|   TOK_OB expression_binary TOK_CB {
    $$ = $2;
    }
|   expression_binary {
    $$ = $1;
    }
|   variable {
    $$ = sat_new_leaf_expression_node($1);
    }
;

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

unary_constraints : 
| unary_constraint unary_constraints
;

unary_constraint  :
    variable TOK_OP_EQ TOK_ZERO {
        $1 -> can_be_0 = SAT_TRUE;
        $1 -> can_be_1 = SAT_FALSE;
    }
|   variable TOK_OP_EQ TOK_ONE  {
        $1 -> can_be_0 = SAT_FALSE;
        $1 -> can_be_1 = SAT_TRUE;
    }
|   variable TOK_OP_NE TOK_ZERO {
        $1 -> can_be_0 = SAT_FALSE;
    }
|   variable TOK_OP_NE TOK_ONE {
        $1 -> can_be_1 = SAT_FALSE;
    }
;

expectations:
|   domain_expectation expectations
;

domain_expectation:
    TOK_EXPECT TOK_DOMAIN TOK_ID TOK_OP_EQ TOK_OP TOK_CP {
        sat_expression_variable * vv = sat_new_named_expression_variable($3);
        vv -> check_domain = SAT_TRUE;
        vv -> expect_0     = SAT_FALSE;
        vv -> expect_1     = SAT_FALSE;
}
|   TOK_EXPECT TOK_DOMAIN TOK_ID TOK_OP_EQ TOK_OP TOK_ONE  TOK_CP{
        sat_expression_variable * vv = sat_new_named_expression_variable($3);
        vv -> check_domain = SAT_TRUE;
        vv -> expect_0     = SAT_FALSE;
        vv -> expect_1     = SAT_TRUE;
}
|   TOK_EXPECT TOK_DOMAIN TOK_ID TOK_OP_EQ TOK_OP TOK_ZERO TOK_CP{
        sat_expression_variable * vv = sat_new_named_expression_variable($3);
        vv -> check_domain = SAT_TRUE;
        vv -> expect_0     = SAT_TRUE;
        vv -> expect_1     = SAT_FALSE;
}
|   TOK_EXPECT TOK_DOMAIN TOK_ID TOK_OP_EQ TOK_OP TOK_ZERO TOK_ONE TOK_CP{
        sat_expression_variable * vv = sat_new_named_expression_variable($3);
        vv -> check_domain = SAT_TRUE;
        vv -> expect_0     = SAT_TRUE;
        vv -> expect_1     = SAT_TRUE;
}

%%

int yyerror (const char *s)
{
      printf ("[PARSE ERROR] %s\n", s);
      return 1;
}


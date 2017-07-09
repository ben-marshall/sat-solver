

%{

#include <stdio.h>

#include "sat_expression_parser.h"

%}

%option yylineno
%option nodefault 
%option noyywrap 

DIGIT    [01]
ID       [2-9][0-9]*

END      END

ASSIGN   =

OP_EQ    ==
OP_AND   &
OP_OR    \|
OP_NOT   ~
OP_XOR   \^

OB       \(
CB       \)

%%

{END}  {
    yyterminate();
}

{DIGIT} {
    return TOK_DIGIT;
}

{ID} {
    return TOK_ID;
}

{OB} {
    return TOK_OB;
} 
{CB} {
    return TOK_CB;
}

{OP_EQ} {
    return TOK_OP_EQ;
}

{OP_AND} {
    return TOK_OP_AND;
}

{OP_OR} {
    return TOK_OP_OR;
}

{OP_NOT} {
    return TOK_OP_NOT;
}

{OP_XOR} {
    return TOK_OP_XOR;
}

{ASSIGN} {
    return TOK_ASSIGN;
}

[ \t\n]+          /* eat up whitespace */

. {
    printf( "[ERROR] Unrecognized character: %s\n", yytext );
    yyterminate();
}

%%

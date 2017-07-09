

%{

#include <stdio.h>

%}

%option yylineno
%option nodefault 
%option noyywrap 

DIGIT    [01]
ID       [a-zA-Z][a-zA-Z0-9_]*

END      END

ASSIGN   =

OP_EQ    ==
OP_AND   &
OP_OR    |
OP_NOT   ~
OP_XOR   ^

OB       (
CB       )

%%

{END}  {
    yyterminate();
}

{DIGIT}    {
    return TOK_DIGIT;
}

{ID} {
    return TOK_ID;
}

{OB}  {
    return TOK_OB;
} 
{CB} {
    return TOK_CB;
}

{OP_EQ} {
    return TOK_EQ;
}

{OP_AND} {
    return TOK_AND;
}

{OP_OR} {
    return TOK_OR;
}

{OP_NOT} {
    return TOK_NOT;
}

{OP_XOR} {
    return TOK_XOR;
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

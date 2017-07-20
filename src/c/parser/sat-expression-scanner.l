

%{

#include <stdio.h>
#include <string.h>

#include "sat-expression.h"
#include "sat-expression-parser.h"

%}

%option yylineno
%option nodefault 
%option noyywrap 

ID       [a-zA-Z][a-zA-Z_0-9]*

END      end

ASSIGN   =

ZERO     0
ONE      1

OP_NE    !=
OP_EQ    ==
OP_AND   &
OP_OR    \|
OP_NOT   ~
OP_XOR   \^

OB       \(
CB       \)

%%

{END}  {
    return TOK_END;
}

{ZERO} {
    return TOK_ZERO;
}

{ONE} {
    return TOK_ONE;
}

{ID} {
    size_t text_len = strlen(yytext);
    yylval.vid = calloc(text_len, sizeof(char));
    memcpy(yylval.vid, yytext,text_len);
    return TOK_ID;
}

{OB} {
    return TOK_OB;
} 
{CB} {
    return TOK_CB;
}

{OP_NE} {
    return TOK_OP_NE;
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

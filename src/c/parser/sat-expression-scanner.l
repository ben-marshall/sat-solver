

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

OP_OR    \|
OP_NOR   ~\|
OP_XOR   \^
OP_NXOR  ~\^
OP_NE    !=
OP_EQ    ==
OP_AND   &
OP_NAND  ~&
OP_NOT   ~
OP_IMP   ->

OB       \(
CB       \)

EXPECT   expect
DOMAIN   domain

OP       \{
CP       \}

COMMENT  \/\/.*\n

%%

{COMMENT}

{END}  {
    return TOK_END;
}

{ZERO} {
    return TOK_ZERO;
}

{ONE} {
    return TOK_ONE;
}

{EXPECT} {
    return TOK_EXPECT;
}

{DOMAIN} {
    return TOK_DOMAIN;
}
{ID} {
    size_t text_len = strlen(yytext);
    yylval.vid = calloc(text_len+1, sizeof(char));
    memcpy(yylval.vid, yytext,text_len);
    return TOK_ID;
}
{OP} {
    return TOK_OP;
} 
{CP} {
    return TOK_CP;
}
{OB} {
    return TOK_OB;
} 
{CB} {
    return TOK_CB;
}

{OP_OR} {    return TOK_OP_OR;}
{OP_NOR} {   return TOK_OP_NOR;}
{OP_XOR} {    return TOK_OP_XOR;}
{OP_NXOR} {   return TOK_OP_NXOR;}
{OP_NE} {    return TOK_OP_NE;}
{OP_EQ} {    return TOK_OP_EQ;}
{OP_AND} {    return TOK_OP_AND;}
{OP_NAND} {    return TOK_OP_NAND;}
{OP_NOT} {    return TOK_OP_NOT;}
{OP_IMP} {    return TOK_OP_IMP;}

{ASSIGN} {
    return TOK_ASSIGN;
}

[ \t\n]+          /* eat up whitespace */

. {
    printf( "[ERROR] Unrecognized character: %s\n", yytext );
    yyterminate();
}

%%

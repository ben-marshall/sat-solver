
#ifndef H_SATSOLVER
#define H_SATSOLVER

#include "imp-matrix.h"

/*!
@brief Describes the types of token we might find in a SAT expression
*/
typedef enum {
    TOK_DIGIT  = 1 ,
    TOK_ID     = 2 ,
    TOK_END    = 3 ,
    TOK_ASSIGN = 4 ,
    TOK_OP_EQ  = 5 ,
    TOK_OP_AND = 6 ,
    TOK_OP_OR  = 7 ,
    TOK_OP_XOR = 8 ,
    TOK_OP_NOT = 9 ,
    TOK_OB     = 10,
    TOK_CB     = 11
} t_sat_expression_token;

#endif


#include "imp-matrix.h"

#ifndef H_SATEXPRESSION
#define H_SATEXPRESSION

//! @brief Describes a type of operation on binary variables.
typedef enum t_sat_operation{
    OP_AND,
    OP_OR,
    OP_XOR,
    OP_EQ,
    OP_NOT
} sat_operation;


typedef struct t_sat_binary_expression sat_binary_expression;

/*!
@brief Describes a single binary expression
*/
struct t_sat_binary_expression {
    t_sat_var  assigne; //!< The variable assigned to.
    t_sat_var  lhs;     //!< Variable on the left side of the operator.
    t_sat_var  rhs;     //!< Variable on the right side of the operator.
    sat_operation op;    //!< The operation performed.
    sat_binary_expression * next;
};



/*!
@brief Create a new binary expression object.
@param [in] assigne - The variable assigned to.
@param [in] lhs     - Variable on the left side of the operator.
@param [in] rhs     - Variable on the right side of the operator.
@param [in] op      - The operation performed.
@returns A pointer to the new expression object.
*/
sat_binary_expression * sat_new_binary_expression(
    t_sat_var  assigne,
    t_sat_var  lhs,    
    t_sat_var  rhs,    
    sat_operation op   
);

//!@brief Frees a binary expression.
void sat_free_binary_expression(
    sat_binary_expression * tofree
);


#endif


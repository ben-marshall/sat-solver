
#include "sat-expression.h"


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
){
    sat_binary_expression * tr = calloc(1, sizeof(sat_binary_expression));

    tr -> assigne = assigne;
    tr -> lhs = lhs;
    tr -> rhs = rhs;
    tr -> op  = op;
    
    return tr;
}


/*!
@brief Create a new unary expression object.
@param [in] assigne - The variable assigned to.
@param [in] rhs     - Variable on the right side of the operator.
@param [in] op      - The operation performed.
@returns A pointer to the new expression object.
*/
sat_unary_expression * sat_new_unary_expression(
    t_sat_var  assigne,
    t_sat_var  rhs,    
    sat_operation op   
){
    sat_unary_expression * tr = calloc(1, sizeof(sat_unary_expression));

    tr -> assigne = assigne;
    tr -> rhs = rhs;
    tr -> op  = op;
    
    return tr;
}

//!@brief Frees a binary expression.
void sat_free_binary_expression(
    sat_binary_expression * tofree
){
    free(tofree);
}   


//!@brief Frees a unary expression.
void sat_free_unary_expression(
    sat_unary_expression * tofree
){
    free(tofree);
}

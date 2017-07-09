
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

//!@brief Frees a binary expression.
void sat_free_binary_expression(
    sat_binary_expression * tofree
){
    free(tofree);
}   


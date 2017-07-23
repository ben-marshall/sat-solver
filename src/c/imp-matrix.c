
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "imp-matrix.h"


/*!
@warning Asserts that variable_count > 0
*/
sat_imp_matrix * sat_new_imp_matrix(
    unsigned int variable_count
){
    assert(variable_count > 0);
    
    sat_imp_matrix * to_return;

    to_return = calloc(1, sizeof(sat_imp_matrix));
        
    to_return -> variable_count = variable_count;
    
    to_return -> domain_0 = calloc(variable_count, sizeof(t_sat_bool));
    to_return -> domain_1 = calloc(variable_count, sizeof(t_sat_bool));
    to_return -> lhs      = calloc(variable_count, sizeof(sat_var_idx));
    to_return -> rhs      = calloc(variable_count, sizeof(sat_var_idx));
    to_return -> op       = calloc(variable_count, sizeof(sat_binary_op));

    return to_return;
}


/*!
@warning Asserts that imp_mat != NULL
*/
void sat_free_imp_matrix(
    sat_imp_matrix * imp_mat
) {
    
    assert(imp_mat != NULL);

    free(imp_mat -> domain_0);
    free(imp_mat -> domain_1);
    free(imp_mat -> lhs     );
    free(imp_mat -> rhs     );
    free(imp_mat -> op      );

    free (imp_mat);
    return;
}



/*!
@brief Add a single relationship between three variables.
@details Add a constraint to say that the `assignee` takes the value of
the operation `op` over the two variables `lhs` and `rhs`:

    assignee = lhs op rhs

Where op can be any member of sat_binary_op.

Setting op as a unary operation (SAT_IMP) simply means that the value of
lhs is ignored.
@param in imp_mat - The matrix to operate on.
@param in assignee - The variable being assigned to
@param in lhs - The left hand side of the operation.
@param in op - Infix operation to perform on lhs, rhs.
@param in rhs - The right hand side of the operation.
@returns void
*/
void sat_add_relation (
    sat_imp_matrix * imp_mat,
    sat_var_idx      assignee,
    sat_var_idx      lhs,
    sat_binary_op    op ,
    sat_var_idx      rhs
){
    
    imp_mat -> lhs[assignee] = lhs;
    imp_mat -> rhs[assignee] = rhs;
    imp_mat -> op [assignee] = op;

}


/*!
@brief Sets a variable up as an input to the system.
@details This has implications on how constraints are propagated later on.
@param in imp_mat - The matrix to operate on.
@param in variable - The variable to mark as an input.
@returns void
*/
void sat_set_as_input(
    sat_imp_matrix * imp_mat,
    sat_var_idx      variable
){
    
    imp_mat -> lhs[variable] = 0;
    imp_mat -> rhs[variable] = 0;
    imp_mat -> op [variable] = SAT_INPUT;
}


/*!
@brief Return true iff the supplied variable is an input to the system.
@param in imp_mat - The matrix to operate on.
@param in variable - The variable to check.
@returns SAT_TRUE if the variable is an input to the system.
*/
t_sat_bool sat_is_input(
    sat_imp_matrix * imp_mat,
    sat_var_idx      variable
) {
    return imp_mat -> op[variable] == SAT_INPUT;
}



/*!
@brief Set the domain for a particular variable.
@details Sets which values a variable may take. Used to apply unary constraints
@param in imp_mat - The matrix to operate on.
@param in variable - The variable who's domain we are updating.
@param in can_be_0 - can the varible take the value False?
@param in can_be_1 - can the varible take the value True?
@returns
*/
void sat_set_domain(
    sat_imp_matrix * imp_mat,
    sat_var_idx      variable,
    sat_var_idx      can_be_0,
    sat_var_idx      can_be_1
) {

    imp_mat -> domain_0[variable] = can_be_0;
    imp_mat -> domain_1[variable] = can_be_1;
}


/*!
@brief Solve the constraint problem represented by the supplied matrix.
@param in imp_mat - The matrix to operate on.
@returns True if the system is solvable.
*/
t_sat_bool sat_solve(
    sat_imp_matrix * imp_mat
) {


    return SAT_FALSE;
}

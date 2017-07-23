
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

    unsigned int i = 0;
    for(i = 0; i < variable_count; i +=1){
        to_return -> domain_0[i] = SAT_TRUE;
        to_return -> domain_1[i] = SAT_TRUE;
    }

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
    
    printf("Add relation %d between %d, %d, %d\n",op, assignee,lhs,rhs);
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
@brief Get the domain for a particular variable.
@param in imp_mat - The matrix to operate on.
@param in variable - The variable who's domain we are updating.
@returns True if the suppled value is in the domain of the variable.
*/
t_sat_bool sat_value_in_domain(
    sat_imp_matrix * imp_mat,
    sat_var_idx      variable,
    t_sat_bool       value
) {
    return value ? imp_mat -> domain_1[variable] :
                   imp_mat -> domain_0[variable] ;
}


/*!
@brief Check the domain for a particular variable is empty.
@param in imp_mat - The matrix to operate on.
@param in variable - The variable who's domain we are querying.
@returns True if the variable has an empty domain.
*/
t_sat_bool sat_domain_empty(
    sat_imp_matrix * imp_mat,
    sat_var_idx      variable
){
    return !(imp_mat -> domain_1[variable] ||
             imp_mat -> domain_0[variable]  );

}


/*!
@brief Checks a single relation is satisfiable.
*/
t_sat_bool sat_solve_arc_reduce(
    sat_imp_matrix * imp_mat,
    sat_var_idx      rel 
){
    
    sat_var_idx lhs = imp_mat -> lhs[rel];
    sat_var_idx rhs = imp_mat -> rhs[rel];

    //printf("%d %d %d\n", rel, lhs, rhs);

    t_sat_bool d0     = imp_mat -> domain_0[rel];
    t_sat_bool d1     = imp_mat -> domain_1[rel];

    t_sat_bool l_cb0  = imp_mat -> domain_0[lhs];
    t_sat_bool l_cb1  = imp_mat -> domain_1[lhs];
    
    t_sat_bool r_cb0  = imp_mat -> domain_0[rhs];
    t_sat_bool r_cb1  = imp_mat -> domain_1[rhs];
    
    t_sat_bool acb0 = d0;
    t_sat_bool acb1 = d1;

    printf("rel %d - d0 = %d, d1 = %d\n",rel ,d0,d1);
    printf("lhs %d - d0 = %d, d1 = %d\n",lhs ,l_cb0, l_cb1);
    printf("rhs %d - d0 = %d, d1 = %d\n",rhs ,r_cb0, r_cb1);

    switch(imp_mat -> op[rel]) {
        case(SAT_OR):
            acb0 = l_cb0 && r_cb0;
            acb1 = l_cb1 || r_cb1;
            break;
        case(SAT_NOR):
            acb0 = l_cb1 || r_cb1;
            acb1 = l_cb0 && r_cb0;
            break;
        case(SAT_XOR):
            acb0 = (l_cb1 && r_cb1) || (l_cb0 && r_cb0);
            acb1 = (l_cb0 && r_cb1) || (l_cb1 && r_cb0);
            break;
        case(SAT_NXOR):
            acb0 = (l_cb0 && r_cb1) || (l_cb1 && r_cb0);
            acb1 = (l_cb1 && r_cb1) || (l_cb0 && r_cb0);
            break;
        case(SAT_AND):
            acb0 = l_cb0 || r_cb0;
            acb1 = l_cb1 && r_cb1;
            break;
        case(SAT_NAND):
            printf("%d = %d NAND %d\n", rel, lhs, rhs);
            acb0 = l_cb1 && r_cb1;
            acb1 = l_cb0 || r_cb0;
            break;
        case(SAT_IMP ):
            acb0 = l_cb1 && r_cb0;
            acb1 = l_cb0 || (l_cb1 && r_cb1);
            break;
        case(SAT_EQ):
            printf("%d EQ %d\n", rel, rhs);
            acb0 = r_cb0;
            acb1 = r_cb1;
            break;
        case(SAT_INPUT):
        default: 
            printf("%d INPUT\n", rel);
            acb0 = d0;
            acb1 = d1;
            break;
    }

    printf("rel %d - acb0 = %d, acb1 = %d\n",rel ,acb0,acb1);
    imp_mat -> domain_0[rel] = acb0;
    imp_mat -> domain_1[rel] = acb1;

    return (acb0 != d0) || (acb1 != d1);
}


/*!
@brief Solve the constraint problem represented by the supplied matrix.
@param in imp_mat - The matrix to operate on.
@returns True if the system is solvable.
*/
t_sat_bool sat_solve(
    sat_imp_matrix * imp_mat
) {
    
    queue * worklist = queue_new();

    sat_var_idx i = 0;
    for (i = 0; i < imp_mat -> variable_count; i +=1) {
        sat_var_idx * vid = calloc(1, sizeof(sat_var_idx));
        vid[0] = i;
        queue_enqueue(worklist, vid);
    }

    while(worklist -> length > 0) {
        printf("\n");

        sat_var_idx * v = queue_dequeue(worklist);
        sat_var_idx relation = v[0];
        free(v);

        if(sat_solve_arc_reduce(imp_mat,relation)) {
            if(sat_domain_empty(imp_mat,relation))
            {
                // Fail
                free(worklist);
                printf("Failed on variable %d\n",relation);
                return SAT_FALSE;
            }
            else
            {
                // Checked relation a on x y
                // Now check all relations w,z on a
                for (i = 0; i < imp_mat -> variable_count; i +=1) {

                    if(imp_mat -> lhs[i] == relation ||
                       imp_mat -> rhs[i] == relation) {
                        
                        printf("Adding %d\n", i);

                        sat_var_idx * vid = calloc(1, sizeof(sat_var_idx));
                        vid[0] = i;
                        queue_enqueue(worklist, vid);
                    }
                }
            }
        }
    }

    free(worklist);
    return SAT_TRUE;
}

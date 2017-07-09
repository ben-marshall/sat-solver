
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
    
    size_t matsize = variable_count * variable_count;

    to_return = calloc(1, sizeof(sat_imp_matrix));
    
    if(to_return == NULL) {

        return NULL;

    } else {
        
        to_return -> variable_count = variable_count;
        to_return -> cell_count     = matsize;
        to_return -> cells          = calloc(
                                        matsize, 
                                        sizeof(sat_imp_matrix_cell)
                                      );

        if(to_return -> cells == NULL) {
            
            free(to_return);

            return NULL;

        } else {

            return to_return;

        }
    }
}



/*!
@warning Asserts that imp_mat != NULL
*/
void sat_free_imp_matrix(
    sat_imp_matrix * imp_mat
) {
    
    assert(imp_mat != NULL);

    if(imp_mat -> cells != NULL) {

        free (imp_mat -> cells);

    }

    free (imp_mat);
    return;
}



/*!
@warning
- Asserts that imp_mat != NULL
- Asserts that implyer_a and implyee_b are both variables contained within the
  supplied imp_mat.
*/
sat_imp_matrix_cell * sat_get_imp_matrix_cell(
    sat_imp_matrix * imp_mat,
    t_sat_var        implyer_a,
    t_sat_var        implyee_b
){
    assert(imp_mat != NULL);
    assert(imp_mat -> variable_count > implyer_a);
    assert(imp_mat -> variable_count > implyee_b);

    unsigned long index = (implyer_a * imp_mat -> variable_count) + implyee_b;
    
    // Return a pointer to the addressed cell.
    return &(imp_mat -> cells[index]);
}


/*!
@warning
- Asserts that imp_mat != NULL
*/
void sat_clear_imp_matrix(
    sat_imp_matrix * imp_mat
){
    assert(imp_mat != NULL);

    unsigned long index = 0;

    for(index = 0; index < imp_mat -> cell_count; index += 1) {
        imp_mat -> cells[index].a_imp_b      = 0;
        imp_mat -> cells[index].a_imp_nb     = 0;
        imp_mat -> cells[index].na_imp_b     = 0;
        imp_mat -> cells[index].na_imp_nb    = 0;
    }

    return;
}



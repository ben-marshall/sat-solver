
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
    t_sat_var_idx        implyer_a,
    t_sat_var_idx        implyee_b
){
    assert(imp_mat != NULL);
    assert(imp_mat -> variable_count > implyer_a);
    assert(imp_mat -> variable_count > implyee_b);

    unsigned long index = (implyer_a * imp_mat -> variable_count) + implyee_b;
    
    // Return a pointer to the addressed cell.
    return &(imp_mat -> cells[index]);
}


/*!
@warning Asserts that cell != NULL
*/
void sat_set_imp_matrix_cell(
    sat_imp_matrix_cell * cell     ,
    t_sat_bitop           a_imp_b  , 
    t_sat_bitop           a_imp_nb , 
    t_sat_bitop           na_imp_b , 
    t_sat_bitop           na_imp_nb 
) {
    assert(cell != NULL);

    switch (a_imp_b) {
        case BITOP_SET:     cell -> a_imp_b = 1; break;
        case BITOP_CLEAR:   cell -> a_imp_b = 0; break;
        case BITOP_TOGGLE:  cell -> a_imp_b = !(cell -> a_imp_b); break;
    }

    switch (a_imp_nb) {
        case BITOP_SET:     cell -> a_imp_nb = 1; break;
        case BITOP_CLEAR:   cell -> a_imp_nb = 0; break;
        case BITOP_TOGGLE:  cell -> a_imp_nb = !(cell -> a_imp_nb); break;
    }

    switch (na_imp_b) {
        case BITOP_SET:     cell -> na_imp_b = 1; break;
        case BITOP_CLEAR:   cell -> na_imp_b = 0; break;
        case BITOP_TOGGLE:  cell -> na_imp_b = !(cell -> na_imp_b); break;
    }

    switch (na_imp_nb) {
        case BITOP_SET:     cell -> na_imp_nb = 1; break;
        case BITOP_CLEAR:   cell -> na_imp_nb = 0; break;
        case BITOP_TOGGLE:  cell -> na_imp_nb = !(cell -> na_imp_nb); break;
    }

    return;
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




/*!
@brief Allocates a new sat_consistancy_check object.
@param [in] is_consistant - Was the checked matrix consistant?
@returns A pointer to the new object.
*/
sat_consistancy_check * sat_new_consistancy_check (
    t_sat_bool is_consistant
){
    sat_consistancy_check * tr = calloc(1, sizeof(sat_consistancy_check));

    if(tr == NULL) {
     
        return NULL;

    } else {
        
        tr -> is_consistant = is_consistant;
        return tr;

    }
}


/*!
@warning Asserts to_free != NULL
*/
void sat_free_consistancy_check (
    sat_consistancy_check * to_free
) {
    assert(to_free != NULL);

    free(to_free);
}



/*!
*/
inline t_sat_bool sat_check_imp_matrix_cell (
    sat_imp_matrix_cell to_check
) {
    return !(
        (to_check.a_imp_b  && to_check.a_imp_nb ) ||
        (to_check.na_imp_b && to_check.na_imp_nb)
    ) ;
}



/*!
@warning
- Asserts that imp_mat != NULL
*/
sat_consistancy_check * sat_check_imp_matrix (
    sat_imp_matrix      * imp_mat,
    t_sat_bool            exit_on_first
) {
    
    assert(imp_mat != NULL);

    sat_consistancy_check * tr = sat_new_consistancy_check(SAT_TRUE);

    t_sat_bool first_fail = SAT_TRUE;
    unsigned long index = 0;

    for(index = 0; index < imp_mat -> cell_count; index += 1) {

        sat_imp_matrix_cell to_check = imp_mat -> cells[index];

        t_sat_bool is_consistant = sat_check_imp_matrix_cell(to_check);

        if(!is_consistant) {
            tr -> is_consistant = SAT_FALSE;

            if(first_fail) {
                tr -> first_failed_implyer = index / imp_mat -> variable_count;
                tr -> first_failed_implyee = index % imp_mat -> variable_count;
                first_fail = SAT_FALSE;
            }
            
            if(exit_on_first) {
                return tr;
            }
        }
    }

    return tr;
}

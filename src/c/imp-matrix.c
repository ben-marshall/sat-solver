
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "imp-matrix.h"


/*!
@brief Create a new relation.
*/
sat_relation * sat_new_relation(
    sat_var_idx     implyer, //<! The variable on the LHS of the implication.
    sat_var_idx     implyee  //<! The variable on the RHS of the implication.
){
    sat_relation * tr = calloc(1, sizeof(sat_relation));

    tr -> implyer = implyer;
    tr -> implyee = implyee;

    return tr;
}


/*!
@brief Free a relation.
*/
void sat_free_relation(
    sat_relation * tofree  //<! The relation to free.
){
    free(tofree);
}


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
        to_return -> d_0            = calloc(
                                        variable_count,
                                        sizeof(t_sat_bool)
                                      );
        to_return -> d_1            = calloc(
                                        variable_count,
                                        sizeof(t_sat_bool)
                                      );
        to_return -> is_input       = calloc(
                                        variable_count,
                                        sizeof(t_sat_bool)
                                      );
        
        unsigned int i = 0;
        for(i = 0; i < variable_count; i ++) {
            to_return -> d_0[i]     = SAT_TRUE;
            to_return -> d_1[i]     = SAT_TRUE;
            to_return -> is_input[i]= SAT_TRUE;
        }

        return to_return;
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

        free (imp_mat -> cells   );
        free (imp_mat -> d_0     );
        free (imp_mat -> d_1     );
        free (imp_mat -> is_input);

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
    sat_var_idx        implyer_a,
    sat_var_idx        implyee_b
){
    assert(imp_mat != NULL);
    assert(imp_mat -> variable_count > implyer_a);
    assert(imp_mat -> variable_count > implyee_b);

    unsigned long index = (implyer_a * imp_mat -> variable_count) + implyee_b;
    imp_mat -> cells[index].implyer = implyer_a;
    imp_mat -> cells[index].implyee = implyee_b;
    
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

    //printf("(%d,%d) - %d %d %d %d\n", cell -> implyer, cell -> implyee,
    //    a_imp_b, a_imp_nb, na_imp_b, na_imp_nb);

    switch (a_imp_b) {
        case BITOP_SET:     cell -> a_imp_b = 1; break;
        case BITOP_CLEAR:   cell -> a_imp_b = 0; break;
        case BITOP_TOGGLE:  cell -> a_imp_b = !(cell -> a_imp_b); break;
        case BITOP_IGN:     break; // Do Nothing.
    }

    switch (a_imp_nb) {
        case BITOP_SET:     cell -> a_imp_nb = 1; break;
        case BITOP_CLEAR:   cell -> a_imp_nb = 0; break;
        case BITOP_TOGGLE:  cell -> a_imp_nb = !(cell -> a_imp_nb); break;
        case BITOP_IGN:     break; // Do Nothing.
    }

    switch (na_imp_b) {
        case BITOP_SET:     cell -> na_imp_b = 1; break;
        case BITOP_CLEAR:   cell -> na_imp_b = 0; break;
        case BITOP_TOGGLE:  cell -> na_imp_b = !(cell -> na_imp_b); break;
        case BITOP_IGN:     break; // Do Nothing.
    }

    switch (na_imp_nb) {
        case BITOP_SET:     cell -> na_imp_nb = 1; break;
        case BITOP_CLEAR:   cell -> na_imp_nb = 0; break;
        case BITOP_TOGGLE:  cell -> na_imp_nb = !(cell -> na_imp_nb); break;
        case BITOP_IGN:     break; // Do Nothing.
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
@brief Re-sets the possible domain values of all variables in a matrix to
the specified value.
@details Lets the same matrix be re-used for multiple SAT queries.
*/
void sat_reset_variable_domains(
    sat_imp_matrix * matrix, //<! The matrix to set the domains for.
    t_sat_bool       d_0,    //<! Should 0 be in the variable domains?
    t_sat_bool       d_1     //<! Should 1 be in the variable domains?
){
    assert(matrix != NULL);

    sat_var_idx idx = 0;

    #pragma omp parallel for num_threads(2) default(shared) schedule(guided)
    for (idx = 0; idx < matrix -> variable_count; idx ++) {
        
        matrix -> d_0[idx] = d_0;
        matrix -> d_1[idx] = d_1;

    }
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


/*!
@brief Given variables A,B,C where some relationships between A,B and B,C
exist, find relationships between A and C.
@param in ab - Variable A -> B
@param in bc - Variable B -> C
@param in ac - Variable A -> C
*/
void sat_propagate_abc(
sat_imp_matrix_cell * ab,
sat_imp_matrix_cell * bc,
sat_imp_matrix_cell * ac
) {
    ac -> a_imp_b   = ac -> a_imp_b  || (ab -> a_imp_b  && bc -> a_imp_b   ) ||
                                        (ab -> a_imp_nb && bc -> na_imp_b  )  ;

    ac -> a_imp_nb  = ac -> a_imp_nb || (ab -> a_imp_b  && bc -> a_imp_nb  ) ||
                                        (ab -> a_imp_nb && bc -> na_imp_nb )  ;

    ac -> na_imp_b  = ac -> na_imp_b || (ab -> na_imp_b  && bc -> a_imp_b  ) ||
                                        (ab -> na_imp_nb && bc -> na_imp_b )  ;

    ac -> na_imp_nb = ac -> na_imp_nb|| (ab -> na_imp_b  && bc -> a_imp_nb ) ||
                                        (ab -> na_imp_nb && bc -> na_imp_nb)  ;
}


/*!
@brief Propagates all implications through the matrix from leaf expressions
to roots.
@param in matrix - Pointer to the implication matrix to perform propagation on.
*/
void sat_propagate_imp_matrix(
    sat_imp_matrix * matrix
){
    sat_var_idx imp_a;
    sat_var_idx imp_b;
    sat_var_idx imp_c;

    for(imp_a = 0; imp_a < matrix -> variable_count ; imp_a += 1) {

        for(imp_b = 0; imp_b < matrix -> variable_count ; imp_b += 1) {
            
            //printf("."); fflush(stdout);

            sat_imp_matrix_cell* a_on_b = sat_get_imp_matrix_cell(matrix,
                                                        imp_a,imp_b);
            
            if(!(a_on_b -> a_imp_b     || a_on_b -> a_imp_nb ||
                 a_on_b -> na_imp_nb   || a_on_b -> na_imp_b )) {
                // No relationship between a and b, so continue.
                continue;
            }
            
#pragma omp parallel for num_threads(2) default(shared) schedule(guided)
            for(imp_c = 0; imp_c < matrix -> variable_count ; imp_c += 1) {
            

                sat_imp_matrix_cell* b_on_c = sat_get_imp_matrix_cell(matrix,
                                                        imp_b,imp_c);
            
                if(!(b_on_c -> a_imp_b     || b_on_c -> a_imp_nb ||
                     b_on_c -> na_imp_nb   || b_on_c -> na_imp_b )) {
                    // No relationship between b and c, so continue.
                    continue;
                }

                // Propagate any implications from a to c through b.
                sat_imp_matrix_cell* a_on_c = sat_get_imp_matrix_cell(matrix,
                                                        imp_a,imp_c);
                
                matrix -> propagation_count++;
                sat_propagate_abc(a_on_b, b_on_c, a_on_c);
            }

        }
        //printf("\n"); fflush(stdout);
    }

}


/*!
@brief Implements the arc-reduce function of AC-3
@param in matrix - The matrix of variable to operate on.
@param in relation - The two related variables to check.
*/
t_sat_bool sat_arc_reduce(
    sat_imp_matrix * matrix,
    sat_relation   * relation
){
    sat_var_idx var_a = relation -> implyer;
    sat_var_idx var_b = relation -> implyee;

    sat_imp_matrix_cell * rel = sat_get_imp_matrix_cell(matrix,var_a, var_b);

    if(rel -> checked) {
        return 0;
    } else {
        rel -> checked = SAT_TRUE;
    }

    t_sat_bool  a_b   = rel -> a_imp_b  ;
    t_sat_bool  a_nb  = rel -> a_imp_nb ;
    t_sat_bool  na_b  = rel -> na_imp_b ;
    t_sat_bool  na_nb = rel -> na_imp_nb;

    if(a_b || a_nb || na_b || na_nb) 
    {

        t_sat_bool  a0    = matrix -> d_0[var_a];
        t_sat_bool  a1    = matrix -> d_1[var_a];

        t_sat_bool  b0    = matrix -> d_0[var_b];
        t_sat_bool  b1    = matrix -> d_1[var_b];

        matrix -> d_0[var_a] =
            (a0 && na_nb && b0) ||
            (a0 && na_b  && b1)  ;
        
        matrix -> d_1[var_a] =
            (a1 && a_nb && b0) ||
            (a1 && a_b  && b1)  ;

        t_sat_bool delta = (matrix -> d_0[var_a] != a0) ||
                           (matrix -> d_1[var_a] != a1)  ;

        printf("Checking %d -> %d : %d\n", var_a, var_b, delta);

    }
    return 0;
}


/*!
@brief Performs a SAT solving pass starting with the supplied variable.
@param in matrix - The matrix to do the solving over.
@param in var    - Variable index to start with.
@returns True if the start variable is satisfiable, False if not.
*/
t_sat_bool sat_solve(
    sat_imp_matrix * matrix,
    sat_var_idx      start
) {
    
    t_sat_bool result = SAT_TRUE;

    // Construct the initial list of relations to deal with.
    queue * worklist = queue_new();
    
    sat_var_idx i = 0;
    for(i = 0; i < matrix -> variable_count; i ++) {
        // Add all relations start -> X where X is any other variable to the
        // work list.
        if(i != start ) {
            sat_relation * toadd = sat_new_relation(start ,i);
            queue_enqueue(worklist, (void*) toadd);
        }
    }

    printf("Solve - Initial worklist size: %d\n", worklist -> length);
    
    // While the work list is not empty.
    while (worklist -> length > 0) {

        sat_relation * tocheck = (sat_relation*)queue_dequeue(worklist);

        sat_var_idx var_x = tocheck -> implyer;
        sat_var_idx var_y = tocheck -> implyee;
        
        t_sat_bool reduce_result = sat_arc_reduce(matrix, tocheck);

        if(reduce_result) {
            
            // Check the domain of the implyer varible of the relation being
            // checked.

            if(!(matrix -> d_0[var_x] ||
                 matrix -> d_1[var_x])) {
                
                // Fail, the implyer has an empty domain!
                result = SAT_FALSE;

            } else {
                
                // We've checked all relations X -> Y for all variables Y.
                // Now check Z -> X for all variables Z.

                for(i = 0; i < matrix -> variable_count; i ++) {
                    if(i != var_y && i != var_x) {
                        sat_relation * toadd = sat_new_relation(i, var_x);
                        queue_enqueue(worklist, toadd);
                    }
                }
                

            }

        }

        sat_free_relation(tocheck);

    }

    return result;
    
}



#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "queue.h"
#include "satsolver.h"

#ifndef H_IMP_MATRIX
#define H_IMP_MATRIX

//  ------------------ Type Definitions ----------------------------------


//! @typedef A simple boolean type used by the sat solver.
typedef unsigned char t_sat_bool;

//! @typedef Numerical representation of a single boolean variable.
typedef unsigned int  sat_var_idx;

//! @typedef A string representing the name of a single boolean variable.
typedef char * sat_var_name;

//  ------------------ Data Structures -----------------------------------

/*!
@brief This structure contains a complete description of an implication
       matrix used to record relationships between variables.
*/
typedef struct s_sat_imp_matrix {
    
    //! The number of variables the matrix holds relations for.
    unsigned int    variable_count;
    
} sat_imp_matrix;




//  ------------------ Function Prototypes -------------------------------


/*!
@brief Allocates a new implication matrix object with the required number
of cells to handle variable_count variables.
@param [in] variable_count - The number of variables the matrix must hold.
@returns A pointer to a newly created implication matrix object or NULL if
the matrix allocation failed.
@see sat_imp_matrix sat_free_imp_matrix
*/
sat_imp_matrix * sat_new_imp_matrix(
    unsigned int variable_count
);



/*!
@brief Frees an implication matrix object from memory.
@param [in] imp_mat - The implication matrix to be free'd.
@returns void
@see sat_imp_matrix sat_new_imp_matrix
*/
void sat_free_imp_matrix(
    sat_imp_matrix * imp_mat
);


#endif

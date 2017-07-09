
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifndef H_IMP_MATRIX
#define H_IMP_MATRIX


//  ------------------ Type Definitions ----------------------------------


//! @typedef A simple boolean type used by the sat solver.
typedef unsigned char t_sat_bool;

//! @typedef Numerical representation of a single boolean variable.
typedef unsigned int  t_sat_var;

//  ------------------ Data Structures -----------------------------------

/*!
@brief Structure containing the the relationship between two variables in
       the implication matrix.
@details
    This structure represents the complete relationship of some variable
    `B`, given the value of `A`. Each member represents some implication
    of the form:
    
    A -> B

    Where A and B can take the value 0 or 1. If a certain value of A
    implies a certain value of B, then the relevent member is set to 1,
    otherwise it is zero.

@note   Some members are mutually exclusive. For example, it is non-sensical
        for A to imply B, and for A to imply NOT B. This is a contradiction
        and only occurs when an expression is not satisfiable.

@see t_sat_bool
*/
typedef struct s_sat_imp_matrix_cell {
    t_sat_bool  a_imp_b   : 1; //!< If A, then B is implied
    t_sat_bool  a_imp_nb  : 1; //!< If A, then not B is implied
    t_sat_bool  na_imp_b  : 1; //!< If not A, then B is implied
    t_sat_bool  na_imp_nb : 1; //!< If not A, then not B is implied
} sat_imp_matrix_cell;



/*!
@brief This structure contains a complete description of an implication
       matrix used to record relationships between variables.
*/
typedef struct s_sat_imp_matrix {
    
    //! The number of variables the matrix holds relations for.
    unsigned int    variable_count;

    //! How many cells are in the matrix. Equal to variable_count**2
    size_t          cell_count;

    /*! 
    @brief The array of cells, indexed row by column, where [row] indicates
           the left hand side of the implication, and [col] indicates the
           right hand size:

           [row] -> [col]
    @see sat_imp_matrix_cell
    */
    sat_imp_matrix_cell * cells;
} sat_imp_matrix;


//  ------------------ Function Prototypes -------------------------------


/*!
@brief Allocates a new implication matrix object with the required number
of cells to handle <variable_count> variables.
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
@param [in] sat_imp_matrix - The implication matrix to be free'd.
@returns void
@see sat_imp_matrix sat_new_imp_matrix
*/
void sat_free_imp_matrix(
    sat_imp_matrix * imp_mat
);


/*!
@brief Returns a pointer to the implication matrix cell for the supplied
       variables.
@param [in] * imp_mat - The matrix to get the cell from.
@param [in] implyer_a - Variable whos value implies something about the other.
@param [in] implyee_b - Variable whos value is implied by the other.
*/
sat_imp_matrix_cell * sat_get_imp_matrix_cell(
    sat_imp_matrix * imp_mat,
    t_sat_var        implyer_a,
    t_sat_var        implyee_b
);


/*!
@brief Removes all implication relationships from a matrix.
@param [in] * imp_mat - The matrix to remove all implications from.
*/
void sat_clear_imp_matrix(
    sat_imp_matrix * imp_mat
);

#endif

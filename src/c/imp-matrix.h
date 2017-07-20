
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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

//! @brief Describes a type of operation on a bit.
typedef enum {
    BITOP_SET = 1,
    BITOP_CLEAR = 2,
    BITOP_IGN    = 0,
    BITOP_TOGGLE = 3
} t_sat_bitop;

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
    sat_var_idx implyer;
    sat_var_idx implyee;
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
    
    //! How many implications were explicitly added to the matrix?
    unsigned int    implication_count;
    //! How many implications were propagated through the matrix?
    unsigned int    propagation_count;;

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

    /*!
    @brief Array of booleans indicating if the i'th variable in the
    implication matrix can be 0.
    */
    t_sat_bool          * d_0;
    
    /*!
    @brief Array of booleans indicating if the i'th variable in the
    implication matrix can be 1.
    */
    t_sat_bool          * d_1;
    
    /*!
    @brief Array of booleans indicating if the i'th variable in the
    matrix is an input to the system - i.e. does not appear on the LHS
    of an assignment.
    */
    t_sat_bool          * is_input;
} sat_imp_matrix;


/*!
@brief Describes the results of checking an implication matrix for
consistancy.
*/
typedef struct s_sat_consistancy_check {

    t_sat_bool  is_consistant;  //<! Is the checked matrix consistant?
    
    sat_var_idx   first_failed_implyer; //<! First inconsistant implyer.
    sat_var_idx   first_failed_implyee; //<! First inconsistant implyee.

} sat_consistancy_check;


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


/*!
@brief Re-sets the possible domain values of all variables in a matrix to
the specified value.
@details Lets the same matrix be re-used for multiple SAT queries.
*/
void sat_reset_variable_domains(
    sat_imp_matrix * matrix, //<! The matrix to set the domains for.
    t_sat_bool       d_0,    //<! Should 0 be in the variable domains?
    t_sat_bool       d_1     //<! Should 1 be in the variable domains?
);


/*!
@brief Propagates all implications through the matrix from leaf expressions
to roots.
@param in matrix - Pointer to the implication matrix to perform propagation on.
*/
void sat_propagate_imp_matrix(
    sat_imp_matrix * matrix
);



/*!
@brief Allocates a new sat_consistancy_check object.
@param [in] is_consistant - Was the checked matrix consistant?
@returns A pointer to the new object or NULL if the allocation fails.
*/
sat_consistancy_check * sat_new_consistancy_check (
    t_sat_bool is_consistant
);


/*!
@brief Frees a sat_consistancy_check object from memory.
@param [in] *to_free - The object to free.
*/
void sat_free_consistancy_check (
    sat_consistancy_check * to_free
);


/*!
@brief Returns a pointer to the implication matrix cell for the supplied
       variables.
@param [in] imp_mat - The matrix to get the cell from.
@param [in] implyer_a - Variable whos value implies something about the other.
@param [in] implyee_b - Variable whos value is implied by the other.
*/
sat_imp_matrix_cell * sat_get_imp_matrix_cell(
    sat_imp_matrix * imp_mat,
    sat_var_idx        implyer_a,
    sat_var_idx        implyee_b
);



/*!
@brief Modifies the implication relationships of a cell.
@param [inout] cell   - The cell to modify.
@param [in] a_imp_b   - Set/Clear/Ignore this implication relationship.
@param [in] a_imp_nb  - Set/Clear/Ignore this implication relationship.
@param [in] na_imp_b  - Set/Clear/Ignore this implication relationship.
@param [in] na_imp_nb - Set/Clear/Ignore this implication relationship.
*/
void sat_set_imp_matrix_cell(
    sat_imp_matrix_cell * cell     ,
    t_sat_bitop           a_imp_b  , 
    t_sat_bitop           a_imp_nb , 
    t_sat_bitop           na_imp_b , 
    t_sat_bitop           na_imp_nb 
);


/*!
@brief Removes all implication relationships from a matrix.
@note  Allows a single allocated matrix to be used on a different expression,
       rather than re-allocating a whole new one. 
@param [in] imp_mat - The matrix to remove all implications from.
*/
void sat_clear_imp_matrix(
    sat_imp_matrix * imp_mat
);



/*!
@brief Checks that all implications in a single cell are consistent
@param [in] to_check - The cell to check for consistency.
@returns True if the cell is consistent and False if it is not.
*/
t_sat_bool  sat_check_imp_matrix_cell (
    sat_imp_matrix_cell to_check
);



/*!
@brief Checks that all cells in an implication matrix are consistent
@details If exit_on_first is true, then the function will return on the
first inconsistant cell found. Otherwise it will keep going and counting all
of the inconsistant cells.
@param [in] imp_mat - The matrix to check for consistency.
@param [in] exit_on_first    - Return on the first inconsistent cell.
@returns A sat_consistancy_check object describing the result of the check.
*/
sat_consistancy_check * sat_check_imp_matrix (
    sat_imp_matrix      * imp_mat,
    t_sat_bool            exit_on_first
);

#endif

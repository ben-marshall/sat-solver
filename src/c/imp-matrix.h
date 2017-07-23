
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

//! Describes a single binary operation on two variables.
typedef enum e_sat_binary_op {
    SAT_INPUT =0,   //!< No operation, variable is an input.
    SAT_OR=1,
    SAT_NOR=2,
    SAT_XOR=3,
    SAT_NXOR=4,
    SAT_AND=5,
    SAT_NAND=6,
    SAT_EQ=7,
    SAT_IMP=8         //!< Implies
} sat_binary_op;

//  ------------------ Data Structures -----------------------------------

/*!
@brief This structure contains a complete description of an implication
       matrix used to record relationships between variables.
*/
typedef struct s_sat_imp_matrix {
    
    //! The number of variables the matrix holds relations for.
    unsigned int    variable_count;

    //! Does the domain of a variable include 0?
    t_sat_bool   *  domain_0;
    //! Does the domain of a variable include 1?
    t_sat_bool   *  domain_1;
    //! Variable on LHS of operation.
    sat_var_idx  *  lhs;
    //! variable on RHS of operation
    sat_var_idx  *  rhs;
    //! The operation being performed.
    sat_binary_op * op;
    
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
);


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
);


/*!
@brief Return true iff the supplied variable is an input to the system.
@param in imp_mat - The matrix to operate on.
@param in variable - The variable to check.
@returns SAT_TRUE if the variable is an input to the system.
*/
t_sat_bool sat_is_input(
    sat_imp_matrix * imp_mat,
    sat_var_idx      variable
);



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
);


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
);

/*!
@brief Check the domain for a particular variable is empty.
@param in imp_mat - The matrix to operate on.
@param in variable - The variable who's domain we are querying.
@returns True if the variable has an empty domain.
*/
t_sat_bool sat_domain_empty(
    sat_imp_matrix * imp_mat,
    sat_var_idx      variable
);


/*!
@brief Solve the constraint problem represented by the supplied matrix.
@param in imp_mat - The matrix to operate on.
@returns True if the system is solvable.
*/
t_sat_bool sat_solve(
    sat_imp_matrix * imp_mat
);


#endif


#include "satsolver.h"
#include "imp-matrix.h"

#ifndef H_SATEXPRESSION
#define H_SATEXPRESSION

/*!
@defgroup gr-expressions Expression Tree Representation

@brief Functions and data structures used to represent boolean expressions.

@addtogroup gr-expressions
@{
*/


//! @brief Describes whether a sat expression AST node is a leaf or subnode.
typedef enum t_sat_expression_node_type {
    SAT_EXPRESSION_LEAF,
    SAT_EXPRESSION_NODE
} sat_expression_node_type;


/*!
@brief Returns the total number of leaf and intermediate variables in all
parsed assignments.
@returns unsigned integer.
*/
unsigned int sat_get_variable_count();


/*!
@brief Type representing a single variable within a binary expression.
@details
    This structure represents named and implicit variables within a SAT
    expression. For example:

    a = (b.c)+d

    Results in 5 variables:
    - a, b, c, d
    - The result implied by (b.c), we'll call this E
    
    The result of (E+d) does not imply a variable, this is represented by a.
*/
typedef struct t_sat_expression_variable sat_expression_variable;
struct t_sat_expression_variable {
    sat_var_idx     uid;    //!< Unique identifier of the variable.
    sat_var_name    name;   //!< Friendly name. 'a/b/c/d' in example above.
    sat_expression_variable * next; //!< Next in linked list of variables.

    t_sat_bool      can_be_0; //<! Initial domain constraint on being zero
    t_sat_bool      can_be_1; //<! Initial domain constraint on being one.

    t_sat_bool  check_domain; //<! Should we check the domain of the var?
    t_sat_bool      expect_0; //<! Expect 0 to be in the domain of the var
    t_sat_bool      expect_1; //<! Expect 1 to be in the domain of the var
} ;

/*!
@brief A linked list of all unique expression variables.
@details This is maintained when the sat_new_*_expression_variable function
is called. If the variable being added already exists, it is returned from
this list. Otherwise it is inserted into the list.
*/
sat_expression_variable * yy_sat_variables;


/*!
@brief Create a new un-named SAT expression variable.
@returns A pointer to a newly created sat_expression_variable.
*/
sat_expression_variable * sat_new_expression_variable();


/*!
@brief Create a new named SAT expression variable.
@param [in] name  - Friendly name
@returns A pointer to a newly created sat_expression_variable.
*/
sat_expression_variable * sat_new_named_expression_variable(
    sat_var_name    name
);


/*!
@brief Returns the variable associated with the supplied ID.
@param [in] id - The unique id of the variable.
@returns A pointer to the expression variable or NULL if no such variable
exists.
*/
sat_expression_variable * sat_get_variable_from_id(
    sat_var_idx     id
);


/*!
@brief Free the memory taken up by an expression variable.
@param [in] tofree    - Pointer to the variable to free.
@param [in] freelist  - Should we recursively free the *next field?
@note Also frees the memory allocated for the name field of tofree.
*/
void sat_free_expression_variable (
    sat_expression_variable    * tofree,
    t_sat_bool                   freelist
);


//! @brief Typedef for the sat_expression_node
typedef struct t_sat_expression_node sat_expression_node;
struct t_sat_expression_node {
    sat_expression_node_type    node_type; //!< Is this a node or a leaf?
    sat_binary_op               op_type;   //!< Which operation is the node?
    sat_expression_variable  *  ir; //<! Intermediate result.

    /*!
        @brief Stores the data of the AST node depending on the node and
        operation type.
    */
    union {

        //! @brief Valid iff node_type == SAT_EXPRESSION_LEAF
        sat_expression_variable  *  leaf_variable;

        /*!
            @brief Valid iff:
                node_type == SAT_EXPRESSION_NODE && op_type == SAT_OP_NOT
        */
        struct {
            sat_expression_node  *  lhs;
            sat_expression_node  *  rhs;
        } binary_operands ;
        
        /*!
            @brief Valid iff:
                node_type == SAT_EXPRESSION_NODE && op_type != SAT_OP_NOT
        */
        struct {
            sat_expression_node  *  rhs;
        } unary_operands;
    } node ;
};

/*!
@brief Free the memory taken up by an expression node.
@details Recursively fees this expression node and all sub-expression nodes,
but leaves the leaf variables allocated for later use.
@param [in] tofree    - Pointer to the expression node to free.
*/
void sat_free_expression_node(
    sat_expression_node        * tofree
);


/*!
@brief Create a new sat_expression_node object for a leaf variable.
@param [in] variable - The leaf variable for the node.
@returns A pointer to a newly created sat_expression_node or NULL if the
memory allocation fails.
*/
sat_expression_node * sat_new_leaf_expression_node (
    sat_expression_variable * variable
);


/*!
@brief Create a new sat_expression_node object for a unary operation
@param [in] child - The child node the unary op is performed on.
@param [in] op_type - What sort of operation is being performed?
@returns A pointer to a newly created sat_expression_node or NULL if the
memory allocation fails.
*/
sat_expression_node * sat_new_unary_expression_node (
    sat_expression_node * child,
    sat_binary_op         op_type
);


/*!
@brief Create a new sat_expression_node object for a binary operation
@param [in] lhs - left hand node of the operation
@param [in] rhs - right hand node of the operation
@param [in] op_type - What sort of operation is being performed?
@returns A pointer to a newly created sat_expression_node or NULL if the
memory allocation fails.
*/
sat_expression_node * sat_new_binary_expression_node (
    sat_expression_node * lhs,
    sat_expression_node * rhs,
    sat_binary_op         op_type
);

//! Typedef for representing a single assignment to a single variable.
typedef struct t_sat_assignment sat_assignment;

/*!
@brief Global variable containing a linked list of assignment expressions.
*/
sat_assignment * yy_assignments;

struct t_sat_assignment {
    sat_expression_variable * variable;   //!< The variable being assigned to.
    sat_expression_node     * expression; //!< Expression whoes value to take.
    sat_assignment          * next; //!< Next in linked list.
};


/*!
@brief Create a new assignment of an expression to a variable.
@returns a pointer to the new assignment or NULL if the assignment fails.
*/
sat_assignment * sat_new_assignment (
    sat_expression_variable * variable,   //!< The variable being assigned to.
    sat_expression_node     * expression  //!< Expression whoes value to take.
);

/*!
@brief Frees an assignmnet from memory along with all child expression
data structures. It does *not* free the expression variables however.
Can also free the pointed to <next> sat_assignment member.
@param [in] tofree - pointer to the assignmen to be free'd.
@param [in] freelist - Should we also recursively free the *next item in the
linked list?
*/
void sat_free_assignment(
    sat_assignment * tofree,
    t_sat_bool       freelist
);

/*!
@brief Apply any unary constraints on the value of a variable to an
implication matrix.
*/
void sat_apply_unary_constraints(
    sat_imp_matrix          * matrix,
    sat_expression_variable * var
);

/*!
@brief Takes a single assignment expression and adds it to the implication
matrix.
@param [inout] matrix - The matrix to add the assignment to
@param [in]    toadd  - The assignment to add to the matrix.
*/
void sat_add_assignment_to_imp_matrix(
    sat_imp_matrix * matrix,
    sat_assignment * toadd
);


/*!
@brief Check if the domains of all variables after sat solving match any
prior expectations.
@param [in] variables - Linked list of variables to check.
@param [in] print_failures - If true, print to stdout when we fail expectations
@returns Boolean True indicating all expectations were met. False otherwise.
Returns true if there were no expectations.
*/
t_sat_bool sat_check_expectations(
    sat_expression_variable * variables,
    sat_imp_matrix          * matrix,
    t_sat_bool                print_failures
);

/*! @} */

#endif


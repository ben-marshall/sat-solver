
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "sat-expression.h"

//! Linked list of named variables.
extern sat_expression_variable * yy_sat_variables;


/*!
@brief Create a new un-named SAT expression variable.
@param in id    - Unique identifier
@returns A pointer to a newly created sat_expression_variable.
*/
sat_expression_variable * sat_new_expression_variable(
    sat_var_idx     uid
){
    sat_expression_variable * tr = calloc(1, sizeof(sat_expression_variable));

    if(tr == NULL)
    {
        return NULL;
    }
    else
    {
        tr -> uid  = uid;
        tr -> name = '\0';
        return tr;
    }
}


/*!
@brief Create a new named SAT expression variable.
@param in name  - Friendly name
@param in id    - Unique identifier
@returns A pointer to a newly created sat_expression_variable.
*/
sat_expression_variable * sat_new_named_expression_variable(
    sat_var_idx     uid,
    sat_var_name    name
){
    // Check if a variable with this name already exists.
    sat_expression_variable * walker = yy_sat_variables;

    if(walker == NULL) {

        sat_expression_variable * tr = sat_new_expression_variable(uid);
        tr     -> name = name;
        yy_sat_variables = tr;
        return tr;
    }

    while(walker != NULL) {

        int result = strcmp(name, walker -> name);

        if(result == 0) {
            
            return walker;

        } else if (result < 0) {

            sat_expression_variable * tr = sat_new_expression_variable(uid);
            tr     -> next = walker -> next;
            walker -> next = tr;
            tr     -> name = name;
            return tr;

        } else if (walker -> next == NULL)  {
            
            sat_expression_variable * tr = sat_new_expression_variable(uid);
            walker -> next = tr;
            tr     -> name = name;
            return tr;

        } else {
            walker = walker -> next;
        }
    }
    assert(1==0);
}


/*!
@brief Free the memory taken up by an expression variable.
@param in tofree    - Pointer to the variable to free.
@note Also frees the memory allocated for the name field of tofree.
*/
void sat_free_expression_variable (
    sat_expression_variable    * tofree
){
    if(tofree != NULL)
    {
        if(tofree -> name != NULL) {
            free(tofree -> name);
        }
        free(tofree);
    }
}


/*!
@brief Create a new sat_expression_node object with a given type.
@param in node_type - Is this a leaf node (for a variable) or expression node?
@returns A pointer to a newly created sat_expression_node or NULL if the
memory allocation fails.
*/
sat_expression_node * sat_new_expression_node (
    sat_expression_node_type    node_type
) {
    sat_expression_node * tr = calloc(1, sizeof(sat_expression_node));

    if(tr == NULL)
    {
        return NULL;
    }
    else
    {
        tr -> node_type = node_type;
        return tr;
    }
}



/*!
@brief Create a new sat_expression_node object for a leaf variable.
@param in variable - The leaf variable for the node.
@returns A pointer to a newly created sat_expression_node or NULL if the
memory allocation fails.
*/
sat_expression_node * sat_new_leaf_expression_node (
    sat_expression_variable * variable
) {
    assert(variable != NULL);
    sat_expression_node * tr = calloc(1, sizeof(sat_expression_node));
    tr -> node_type = SAT_EXPRESSION_LEAF;

    if(tr == NULL)
    {
        return NULL;
    }
    else
    {
        tr -> op_type            = SAT_OP_NONE;
        tr -> node.leaf_variable = variable;
        return tr;
    }
}


/*!
@brief Create a new sat_expression_node object for a unary operation
@param in child - The child node the unary op is performed on.
@param in op_type - What sort of operation is being performed?
@returns A pointer to a newly created sat_expression_node or NULL if the
memory allocation fails.
@warning Asserts that op_type is indeed a unary op!
*/
sat_expression_node * sat_new_unary_expression_node (
    sat_expression_node * child,
    sat_operation         op_type
) {
    assert(op_type == SAT_OP_NOT);
    assert(child   != NULL);

    sat_expression_node * tr = calloc(1, sizeof(sat_expression_node));
    tr -> node_type = SAT_EXPRESSION_NODE;

    if(tr == NULL)
    {
        return NULL;
    }
    else
    {
        tr -> op_type                 = op_type;
        tr -> node.unary_operands.rhs = child;
        return tr;
    }
}


/*!
@brief Create a new sat_expression_node object for a binary operation
@param in lhs - left hand node of the operation
@param in rhs - right hand node of the operation
@param in op_type - What sort of operation is being performed?
@returns A pointer to a newly created sat_expression_node or NULL if the
memory allocation fails.
@warning Asserts that op_type is indeed a binary op!
*/
sat_expression_node * sat_new_binary_expression_node (
    sat_expression_node * lhs,
    sat_expression_node * rhs,
    sat_operation         op_type
) {
    assert(rhs != NULL);
    assert(lhs != NULL);
    assert(op_type == SAT_OP_AND ||
           op_type == SAT_OP_OR  ||
           op_type == SAT_OP_XOR );

    sat_expression_node * tr = calloc(1, sizeof(sat_expression_node));
    tr -> node_type = SAT_EXPRESSION_NODE;

    if(tr == NULL)
    {
        return NULL;
    }
    else
    {
        tr -> op_type  = op_type;
        tr -> node.binary_operands.lhs = lhs;
        tr -> node.binary_operands.rhs = rhs;
        return tr;
    }
}


/*!
@brief Create a new assignment of an expression to a variable.
@returns a pointer to the new assignment or NULL if the assignment fails.
*/
sat_assignment * sat_new_assignment (
    sat_expression_variable * variable,   //!< The variable being assigned to.
    sat_expression_node     * expression  //!< Expression whoes value to take.
){
    assert(variable != NULL);
    assert(expression != NULL);

    sat_assignment * tr = calloc(1, sizeof(sat_assignment));

    if(tr == NULL)
    {
        return NULL;
    }
    else
    {
        tr -> expression = expression;
        tr -> variable = variable;
        return tr;
    }
}


/*!
@brief Adds an expression and all sub-expressions into the implication matrix.
@param in depth - How deep is this nested expression? 0 indicates the root.
*/
void sat_add_expression_to_imp_matrix(
    unsigned int          depth,
    sat_imp_matrix      * matrix,
    sat_expression_node * toadd
) {
    
    // We don't need to handle this kind of node.
    if(toadd -> node_type == SAT_EXPRESSION_LEAF) {
        printf("Leafnode %d (%s)\n", toadd -> node.leaf_variable -> uid ,
                                     toadd -> node.leaf_variable -> name);
        return;
    }

    if(toadd -> op_type == SAT_OP_NOT) {

        // We need to handle a UNARY operation.
        sat_add_expression_to_imp_matrix(depth+1,matrix, 
                                         toadd -> node.unary_operands.rhs);
    
    } else if (toadd -> op_type == SAT_OP_AND) {

        // Binary AND OP.
        sat_add_expression_to_imp_matrix(depth+1,matrix, 
                                         toadd -> node.binary_operands.rhs);
        sat_add_expression_to_imp_matrix(depth+1,matrix, 
                                         toadd -> node.binary_operands.lhs);

    } else if (toadd -> op_type == SAT_OP_OR) {

        // Binary OR OP.
        sat_add_expression_to_imp_matrix(depth+1,matrix, 
                                         toadd -> node.binary_operands.rhs);
        sat_add_expression_to_imp_matrix(depth+1,matrix, 
                                         toadd -> node.binary_operands.lhs);

    } else if (toadd -> op_type == SAT_OP_XOR) {

        // Binary XOR OP.
        sat_add_expression_to_imp_matrix(depth+1,matrix, 
                                         toadd -> node.binary_operands.rhs);
        sat_add_expression_to_imp_matrix(depth+1,matrix, 
                                         toadd -> node.binary_operands.lhs);

    } else if (toadd -> op_type == SAT_OP_EQ) {

        // Binary NXOR OP.
        sat_add_expression_to_imp_matrix(depth+1,matrix, 
                                         toadd -> node.binary_operands.rhs);
        sat_add_expression_to_imp_matrix(depth+1,matrix, 
                                         toadd -> node.binary_operands.lhs);

    } else {
        
        // Whaa?
        printf("Warning: Unknown binary op type: '%d'\n", toadd -> op_type);
        return;
        
    }

}


/*!
@brief Takes a single assignment expression and adds it to the implication
matrix.
@param inout matrix - The matrix to add the assignment to
@param in    toadd  - The assignment to add to the matrix.
*/
void sat_add_assignment_to_imp_matrix(
    sat_imp_matrix * matrix,
    sat_assignment * toadd
){
    assert(matrix != NULL);
    assert(toadd  != NULL);

    // First add the expression associated with the assignment.
    sat_add_expression_to_imp_matrix(0,matrix, toadd -> expression);
    
    // Now add the implications for the variable being assigned to.
}


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "sat-expression.h"



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
    sat_expression_variable * tr = sat_new_expression_variable(uid);

    if(tr == NULL)
    {
        return NULL;
    }
    else
    {
        tr -> name = name;
        return tr;
    }
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




#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "sat-expression.h"

//! Incremented every time we declare a new ID.
unsigned int yy_id_counter = 0;

/*!
@brief Returns the total number of leaf and intermediate variables in all
parsed assignments.
@returns unsigned integer.
*/
unsigned int sat_get_variable_count()
{
    return yy_id_counter;
}


/*!
@brief Turns a number into a string to be used as an intermediate result
expression variable name.
@param in id - The number, usually the value of yy_id_counter;
@returns a string with the number in the name and a prefix.
*/
char * sat_expression_var_id_to_name(unsigned int id) {
    
    size_t len = 3 + (int)(ceil(log10(id))+1);

    char * tr = calloc(len, sizeof(char));
    
    sprintf(tr, "iv_%d", id);

    return tr;
}

/*!
@brief Create a new un-named SAT expression variable.
@returns A pointer to a newly created sat_expression_variable.
*/
sat_expression_variable * sat_new_expression_variable( )
{
    sat_expression_variable * tr = calloc(1, sizeof(sat_expression_variable));

    if(tr == NULL)
    {
        return NULL;
    }
    else
    {
        tr -> uid  = yy_id_counter ++;
        tr -> name = NULL;
        return tr;
    }
}


/*!
@brief Create a new named SAT expression variable.
@param in name  - Friendly name
@returns A pointer to a newly created sat_expression_variable.
*/
sat_expression_variable * sat_new_named_expression_variable(
    sat_var_name    name
){
    // Check if a variable with this name already exists.
    sat_expression_variable * walker = yy_sat_variables;

    if(walker == NULL) {

        sat_expression_variable * tr = sat_new_expression_variable();
        tr     -> name = name;
        yy_sat_variables = tr;
        return tr;
    }

    while(walker != NULL) {

        int result = strcmp(name, walker -> name);

        if(result == 0) {
            
            return walker;

        } else if (result < 0) {

            sat_expression_variable * tr = sat_new_expression_variable();
            tr     -> next = walker -> next;
            walker -> next = tr;
            tr     -> name = name;
            return tr;

        } else if (walker -> next == NULL)  {
            
            sat_expression_variable * tr = sat_new_expression_variable();
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
@param in freelist  - Should we recursively free the *next field?
@note Also frees the memory allocated for the name field of tofree.
*/
void sat_free_expression_variable (
    sat_expression_variable    * tofree,
    t_sat_bool                   freelist
){
    if(tofree != NULL)
    {
        if(freelist && tofree -> next != NULL) {
            sat_free_expression_variable(tofree -> next, 1);
        }

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
        
        char * varname = sat_expression_var_id_to_name(yy_id_counter+1);
        tr -> ir = sat_new_named_expression_variable(varname);

        printf("Expression node: %d - %s\n", node_type, varname);

        return tr;
    }
}


/*!
@brief Free the memory taken up by an expression node.
@details Recursively fees this expression node and all sub-expression nodes,
but leaves the leaf and intermediate variables allocated for later use.
@param in tofree    - Pointer to the expression node to free.
*/
void sat_free_expression_node(
    sat_expression_node * tofree
){
    assert(tofree != NULL);

    if(tofree -> node_type == SAT_EXPRESSION_LEAF) {

        // Don't free anything more.

    } else if (tofree -> node_type == SAT_EXPRESSION_NODE) {

        if(tofree -> op_type == SAT_OP_NOT) {

            sat_free_expression_node(tofree -> node.unary_operands.rhs);

        } else {

            sat_free_expression_node(tofree -> node.binary_operands.lhs);
            sat_free_expression_node(tofree -> node.binary_operands.rhs);

        }

    }
    
    free(tofree);

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
    sat_expression_node * tr = sat_new_expression_node(SAT_EXPRESSION_LEAF);

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

    sat_expression_node * tr = sat_new_expression_node(SAT_EXPRESSION_NODE);

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

    sat_expression_node * tr = sat_new_expression_node(SAT_EXPRESSION_NODE);

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
@brief Frees an assignmnet from memory along with all child expression
data structures. It does *not* free the expression variables however.
Can also free the pointed to <next> sat_assignment member.
@param in tofree - pointer to the assignmen to be free'd.
@param in freelist - Should we also recursively free the *next item in the
linked list?
*/
void sat_free_assignment(
    sat_assignment * tofree,
    t_sat_bool       freelist
){
    assert(tofree               != NULL);
    assert(tofree -> expression != NULL);

    if(freelist && tofree -> next != NULL) {
        sat_free_assignment(tofree -> next, SAT_TRUE);
    }
    
    sat_free_expression_node(tofree -> expression);

    free(tofree);
}


/*!
@brief Adds all implications for a NOT expression to the matrix
@param in matrix - The implication matrix to add entries too.
@param in toadd - The expression to add implications for.
*/
void sat_add_implications_for_not_to_matrix(
    sat_imp_matrix      * matrix,
    sat_expression_node * toadd
) {
    assert(toadd -> node_type == SAT_EXPRESSION_NODE);
    assert(toadd -> op_type == SAT_OP_NOT);

    sat_var_idx var_a = toadd -> ir -> uid;
    sat_var_idx var_b = toadd -> node.unary_operands.rhs -> ir -> uid;

    sat_imp_matrix_cell * a_to_b = sat_get_imp_matrix_cell(matrix,var_a,var_b);
    sat_imp_matrix_cell * b_to_a = sat_get_imp_matrix_cell(matrix,var_b,var_a);

    // Implications for a NOT operation.
    sat_set_imp_matrix_cell(a_to_b, BITOP_IGN,BITOP_SET,BITOP_SET,BITOP_IGN);
    sat_set_imp_matrix_cell(b_to_a, BITOP_IGN,BITOP_SET,BITOP_SET,BITOP_IGN);
}


/*!
@brief Adds all implications for a AND expression to the matrix
@param in matrix - The implication matrix to add entries too.
@param in toadd - The expression to add implications for.
*/
void sat_add_implications_for_and_to_matrix(
    sat_imp_matrix      * matrix,
    sat_expression_node * toadd
) {
    assert(toadd -> node_type == SAT_EXPRESSION_NODE);
    assert(toadd -> op_type == SAT_OP_AND);

    sat_var_idx var_c = toadd -> ir -> uid;
    sat_var_idx var_b = toadd -> node.binary_operands.rhs -> ir -> uid;
    sat_var_idx var_a = toadd -> node.binary_operands.lhs -> ir -> uid;

    sat_imp_matrix_cell * c_to_a = sat_get_imp_matrix_cell(matrix,var_c,var_a);
    sat_imp_matrix_cell * c_to_b = sat_get_imp_matrix_cell(matrix,var_c,var_b);
    sat_imp_matrix_cell * b_to_c = sat_get_imp_matrix_cell(matrix,var_b,var_c);
    sat_imp_matrix_cell * a_to_c = sat_get_imp_matrix_cell(matrix,var_a,var_c);
    
    // Implications for an AND operation: c = a & b
    sat_set_imp_matrix_cell(c_to_a,BITOP_SET,BITOP_IGN,BITOP_IGN,BITOP_IGN);
    sat_set_imp_matrix_cell(c_to_b,BITOP_SET,BITOP_IGN,BITOP_IGN,BITOP_IGN);
    sat_set_imp_matrix_cell(a_to_c,BITOP_IGN,BITOP_IGN,BITOP_IGN,BITOP_SET);
    sat_set_imp_matrix_cell(b_to_c,BITOP_IGN,BITOP_IGN,BITOP_IGN,BITOP_SET);
}


/*!
@brief Adds all implications for a or expression to the matrix
@param in matrix - The implication matrix to add entries too.
@param in toadd - The expression to add implications for.
*/
void sat_add_implications_for_or_to_matrix(
    sat_imp_matrix      * matrix,
    sat_expression_node * toadd
) {
    assert(toadd -> node_type == SAT_EXPRESSION_NODE);
    assert(toadd -> op_type == SAT_OP_OR);

    sat_var_idx var_c = toadd -> ir -> uid;
    sat_var_idx var_b = toadd -> node.binary_operands.rhs -> ir -> uid;
    sat_var_idx var_a = toadd -> node.binary_operands.lhs -> ir -> uid;

    sat_imp_matrix_cell * c_to_a = sat_get_imp_matrix_cell(matrix,var_c,var_a);
    sat_imp_matrix_cell * c_to_b = sat_get_imp_matrix_cell(matrix,var_c,var_b);
    sat_imp_matrix_cell * b_to_c = sat_get_imp_matrix_cell(matrix,var_b,var_c);
    sat_imp_matrix_cell * a_to_c = sat_get_imp_matrix_cell(matrix,var_a,var_c);

    // Implications for an OR operation: c = a | b
    sat_set_imp_matrix_cell(c_to_a,BITOP_SET,BITOP_IGN,BITOP_IGN,BITOP_SET);
    sat_set_imp_matrix_cell(c_to_b,BITOP_SET,BITOP_IGN,BITOP_IGN,BITOP_SET);
    sat_set_imp_matrix_cell(a_to_c,BITOP_SET,BITOP_IGN,BITOP_IGN,BITOP_IGN);
    sat_set_imp_matrix_cell(b_to_c,BITOP_SET,BITOP_IGN,BITOP_IGN,BITOP_IGN);
}


/*!
@brief Adds all implications for a leaf variable to the matrix
@param in matrix - The implication matrix to add entries too.
@param in toadd - The leaf expression to add implications for.
*/
void sat_add_implications_for_leaf_to_matrix(
    sat_imp_matrix      * matrix,
    sat_expression_node * toadd
) {
    assert(toadd -> node_type == SAT_EXPRESSION_LEAF);
    assert(toadd -> op_type == SAT_OP_NONE);

    sat_var_idx var_b = toadd ->  ir -> uid;
    sat_var_idx var_a = toadd -> node.leaf_variable -> uid;

    sat_imp_matrix_cell * a_to_b = sat_get_imp_matrix_cell(matrix,var_a,var_b);
    sat_imp_matrix_cell * b_to_a = sat_get_imp_matrix_cell(matrix,var_b,var_a);
    
    sat_set_imp_matrix_cell(a_to_b,BITOP_SET,BITOP_IGN,BITOP_IGN,BITOP_SET);
    sat_set_imp_matrix_cell(b_to_a,BITOP_SET,BITOP_IGN,BITOP_IGN,BITOP_SET);
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
    
    if(toadd -> node_type == SAT_EXPRESSION_LEAF) {
        sat_add_implications_for_leaf_to_matrix(matrix,toadd);
        return;
    }

    else if(toadd -> op_type == SAT_OP_NOT) {

        // We need to handle a UNARY operation.
        sat_add_expression_to_imp_matrix(depth+1,matrix, 
                                         toadd -> node.unary_operands.rhs);

        sat_add_implications_for_not_to_matrix(matrix,toadd);

    } else if (toadd -> op_type == SAT_OP_AND) {

        // Binary AND OP.
        sat_add_expression_to_imp_matrix(depth+1,matrix, 
                                         toadd -> node.binary_operands.rhs);
        sat_add_expression_to_imp_matrix(depth+1,matrix, 
                                         toadd -> node.binary_operands.lhs);

        sat_add_implications_for_and_to_matrix(matrix,toadd);

    } else if (toadd -> op_type == SAT_OP_OR) {

        // Binary OR OP.
        sat_add_expression_to_imp_matrix(depth+1,matrix, 
                                         toadd -> node.binary_operands.rhs);
        sat_add_expression_to_imp_matrix(depth+1,matrix, 
                                         toadd -> node.binary_operands.lhs);
    
        sat_add_implications_for_or_to_matrix(matrix,toadd);

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
@todo Finish implementing this.
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
    sat_var_idx var_b = toadd -> variable -> uid;
    sat_var_idx var_a = toadd -> expression -> ir -> uid;

    sat_imp_matrix_cell * a_to_b = sat_get_imp_matrix_cell(matrix,var_a,var_b);
    sat_imp_matrix_cell * b_to_a = sat_get_imp_matrix_cell(matrix,var_b,var_a);
    
    sat_set_imp_matrix_cell(a_to_b,BITOP_SET,BITOP_IGN,BITOP_IGN,BITOP_SET);
    sat_set_imp_matrix_cell(b_to_a,BITOP_SET,BITOP_IGN,BITOP_IGN,BITOP_SET);
}


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "satsolver.h"
#include "sat_expression_parser.h"

/*!
@brief Prints command line usage options for the program.
@param [in] command_line - The value of argv[0], used to launch the program.
*/
void print_usage(char * command_line) {
    printf("Usage: %s <variable count>\n\n", command_line);
    printf("variable-count - Positive integer indicating the number of \n\
                             variables to be used in the expression.");
    printf("\n");
}
 

/*!
@brief Adds parsed expressions to the implication matrix.
*/
void add_expressions(
    sat_imp_matrix * imp_mat
) {
    unsigned long expression_count = 0;

    sat_binary_expression * walker = yy_expressions;

    sat_imp_matrix_cell * as_l;
    sat_imp_matrix_cell * as_r;
    sat_imp_matrix_cell * l_as;
    sat_imp_matrix_cell * r_as;

    as_l = sat_get_imp_matrix_cell(imp_mat,walker -> assigne, walker -> lhs);
    as_r = sat_get_imp_matrix_cell(imp_mat,walker -> assigne, walker -> rhs);
    l_as = sat_get_imp_matrix_cell(imp_mat, walker -> lhs,walker -> assigne);
    r_as = sat_get_imp_matrix_cell(imp_mat, walker -> rhs,walker -> assigne);

    while(walker != NULL) {

        switch(walker -> op) {
            case (OP_AND):
                sat_set_imp_matrix_cell(as_l,1,0,0,0);
                sat_set_imp_matrix_cell(as_r,1,0,0,0);
                sat_set_imp_matrix_cell(l_as,0,0,0,1);
                sat_set_imp_matrix_cell(r_as,0,0,0,1);
                break;

            case (OP_OR ):
                sat_set_imp_matrix_cell(l_as,1,0,0,0);
                sat_set_imp_matrix_cell(r_as,1,0,0,0);
                break;

            case (OP_XOR):
                assert(1==0); // not supported yet.
                break;

            case (OP_EQ ):
                assert(1==0); // not supported yet.
                break;

            case (OP_NOT):
                sat_set_imp_matrix_cell(as_l,0,1,1,0);
                sat_set_imp_matrix_cell(l_as,0,1,1,0);
                break;
        }

        walker = walker -> next;
        expression_count += 1;
    }

    printf("Parsed %lu Expressions into matrix.\n",expression_count);
}

/*!
@brief The Main entry point function for the wrapper program.
@param [in] argc - Number of input arguments
@param [in] argv - Values of input arguments.
@returns 0 If the program terminates successfully, otherwise 1 to indicate
         an error.
*/
int main (int argc, char ** argv) 
{
    printf("----------[SAT-Solver]----------\n");

    if ( argc <= 1 ) {
        print_usage(argv[0]);
        return 1;
    }

    // Lets try the parser...
    printf("Starting the parser...                   "); fflush(stdout);
    yyparse();
    printf("[DONE]\n");

    if(yy_expressions == NULL) {
        printf("No Expressions were parsed.\n");
        return 0;
    }

    // How many variables will our expression contain?
    unsigned int vcount = atoi(argv[1]);

    // Allocate a new implication matrix.
    sat_imp_matrix * imp_mat = sat_new_imp_matrix(vcount);

    if(imp_mat == NULL) {
        printf("[ERROR] - Could not allocate an implication matrix for %d \
 variables.\n", vcount);
        return 1;
    } else {
        printf("Allocated implication matrix for %u variables.\n", vcount);
    }

    // Make sure the matrix has no implications in it.
    printf("Clearing implication matrix...           "); fflush(stdout);
    sat_clear_imp_matrix(imp_mat);
    printf("[DONE]\n");

    // Do a consistancy check on the empty matrix.
    printf("Checking empty matrix for consistancy... "); fflush(stdout);
    sat_consistancy_check * result = sat_check_imp_matrix(imp_mat, 1);
    printf("[DONE]\n");

    printf("Matrix Consistant: %d\n", result -> is_consistant);
    free(result);

    add_expressions(imp_mat);

    // Do a consistancy check on the filled matrix.
    printf("Checking full matrix for consistancy...  "); fflush(stdout);
    result = sat_check_imp_matrix(imp_mat, 1);
    printf("[DONE]\n");
    

    // Free the implication matrix and return.
    sat_free_imp_matrix(imp_mat);

    return 0;
}

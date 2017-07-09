
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

    while(walker != NULL) {
        int c,a,b;
        c = walker -> assigne;
        a = walker -> lhs;
        b = walker -> rhs;

        as_l = sat_get_imp_matrix_cell(imp_mat,walker->assigne, walker -> lhs);
        as_r = sat_get_imp_matrix_cell(imp_mat,walker->assigne, walker -> rhs);
        l_as = sat_get_imp_matrix_cell(imp_mat,walker->lhs,walker -> assigne);
        r_as = sat_get_imp_matrix_cell(imp_mat,walker->rhs,walker -> assigne);

        switch(walker -> op) {
            case (OP_AND):
                printf("%d = %d & %d\n",c,a,b);
                as_l -> a_imp_b   = BITOP_SET;
                as_r -> a_imp_b   = BITOP_SET;
                r_as -> na_imp_nb = BITOP_SET;
                l_as -> na_imp_nb = BITOP_SET;
                break;

            case (OP_OR ):
                printf("%d = %d | %d\n",c,a,b);
                r_as -> a_imp_b = BITOP_SET;
                l_as -> a_imp_b = BITOP_SET;
                break;

            case (OP_XOR):
                assert(1==0); // not supported yet.
                break;

            case (OP_EQ ):
                assert(1==0); // not supported yet.
                break;

            case (OP_NOT):
                printf("%d = ~%d\n",c,b);
                r_as -> a_imp_nb = BITOP_SET;
                r_as -> na_imp_b = BITOP_SET;
                as_r -> a_imp_nb = BITOP_SET;
                as_r -> na_imp_b = BITOP_SET;
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

    printf("Matrix Consistant: %d\n", result -> is_consistant);
    if(!result -> is_consistant) 
    {
        printf(": %d\n", result -> first_failed_implyer);
        printf(": %d\n", result -> first_failed_implyee);

        sat_imp_matrix_cell * cell = sat_get_imp_matrix_cell(imp_mat,
            result -> first_failed_implyer,
            result -> first_failed_implyee );
        printf("(%d,%d,%d,%d)\n",
            cell -> a_imp_b, cell -> a_imp_nb, cell -> na_imp_b,
            cell -> na_imp_nb);
    }
    
    free(result);
    

    // Free the implication matrix and return.
    sat_free_imp_matrix(imp_mat);

    return 0;
}

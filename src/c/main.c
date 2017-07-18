
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "sat-expression.h"
#include "satsolver.h"
#include "imp-matrix.h"
#include "sat-expression-parser.h"
#include "sat-expression-scanner.h"

extern FILE *       yyin;

/*!
@brief Prints command line usage options for the program.
@param [in] command_line - The value of argv[0], used to launch the program.
*/
void print_usage(char * command_line) {
    printf("Usage: %s <assignments-file>\n\n", command_line);
    printf("assignments-file - file path to list of boolean assignments\n\
                               to process.\n");
                             
    printf("\n");
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

    // Try to open the file containing the list of assignments we will
    // parse.

    char * input_file = argv[1];
    printf("Parsing '%s' ", input_file); fflush(stdout);
    yyset_in(fopen(input_file,"r"));

    if(yyin == NULL) {
        printf("Error: Could not open input file '%s'\n", input_file);
        return 1;
    }

    // Run the parser.
    yyparse();
    printf("[DONE]\n");

    // We are finished with the input now.
    fclose(yyin);
    yylex_destroy();

    // How many variables are there?
    unsigned int variable_count = sat_get_variable_count();
    printf("Total Variables: %d\n", variable_count);

    // Build the implication matrix
    sat_imp_matrix * imp_matrix = sat_new_imp_matrix(variable_count);

    // Make sure the empty matrix is consistant first.
    sat_consistancy_check * result = sat_check_imp_matrix(imp_matrix,1);

    if(! result -> is_consistant) {
        printf("Error: empty matrix not consistant. Internal bug.\n");
        free(result);
        return 1;
    }
    free(result);

    printf("Adding assignments to matrix... "); fflush(stdout);

    // Add all of the assignments to the implication matrix.
    sat_assignment * walker = yy_assignments;
    while(walker != NULL) {
        sat_add_assignment_to_imp_matrix(imp_matrix,walker);
        walker = walker -> next;
    }
    

    printf("[DONE]\n");
    printf("Propagating Implications...     "); fflush(stdout);
    
    // Propagate all implications from leaf inputs to roots of assignments.
    sat_propagate_imp_matrix(imp_matrix);

    printf("[DONE]\n");
    printf("Checking Consistancy...         "); fflush(stdout);
    
    // Check there are no obvious contradictions and that the matrix is
    // consistant.
    result = sat_check_imp_matrix(imp_matrix,1);
    printf("[DONE]\n");

    t_sat_bool is_consistant;

    if(! result -> is_consistant) {
        printf("Error: Matrix not consistant.\n");
        is_consistant = SAT_FALSE;

        sat_expression_variable * var_a = 
                      sat_get_variable_from_id(result -> first_failed_implyer);
        sat_expression_variable * var_b = 
                      sat_get_variable_from_id(result -> first_failed_implyee);

        assert(var_a != NULL);
        assert(var_b != NULL);

        printf("Relationship between %s and %s is inconsistant:\n",
                                               var_a -> name, var_b -> name);

    } else {
        printf("Matrix is consistant!\n");
        is_consistant = SAT_TRUE;
    }
    free(result);
    
    printf("Checking Variable Domains...    "); fflush(stdout);

    // Check which variables (if any) are satisfiable.
    sat_update_imp_matrix_domains(imp_matrix);
    printf("[DONE]\n");

    // ---- End of program. Clean up. --------

    // Free the assignment tree.
    sat_free_assignment(yy_assignments, 1);

    // Free the implication matrix
    sat_free_imp_matrix(imp_matrix);

    // Free the expression variable list.
    sat_free_expression_variable(yy_sat_variables,1);
    
    if(is_consistant) {
        return 0;
    } else {
        return 1;
    }
}

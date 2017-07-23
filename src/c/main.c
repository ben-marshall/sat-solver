
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <sys/time.h>

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
  
    // Try to open the file containing the list of assignments we will
    // parse.

    if(argc == 2)
    {
        char * input_file = argv[1];
        if(input_file[0] == '-') {
            yyset_in(stdin);
        } else {
            printf("Parsing '%s' ", input_file); fflush(stdout);
            yyset_in(fopen(input_file,"r"));

            if(yyin == NULL) {
                printf("Error: Could not open input file '%s'\n", input_file);
                return 1;
            }
        }
    }
    else
    {
        // Read input from stdin
        yyset_in(stdin);
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

    
    // Add all of the assignments to the implication matrix.
    printf("Adding assignments to matrix... "); fflush(stdout);

    sat_assignment * walker = yy_assignments;
    while(walker != NULL) {
        sat_add_assignment_to_imp_matrix(imp_matrix,walker);
        walker = walker -> next;
    }


    // Check if we met our expectations of variable domains.

    t_sat_bool met_expectations = SAT_TRUE;
    sat_var_idx vi;

    for(vi = 0; vi < variable_count; vi ++)
    {
        sat_expression_variable * var = sat_get_variable_from_id(vi);
        met_expectations &= sat_check_expectations(var,imp_matrix,SAT_TRUE);
    }
    


    // ---- End of program. Clean up. --------


    // Free the assignment tree.
    sat_free_assignment(yy_assignments, 1);

    // Free the implication matrix
    sat_free_imp_matrix(imp_matrix);

    // Free the expression variable list.
    sat_free_expression_variable(yy_sat_variables,1);
    
    if(met_expectations) {
        printf("Expectations Met!\n");
        return 0;
    } else {
        printf("Expectations Not Met!\n");
        return 1;
    }
}

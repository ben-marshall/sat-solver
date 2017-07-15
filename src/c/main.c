
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "sat-expression.h"
#include "satsolver.h"
#include "imp-matrix.h"
#include "parser/sat-expression-parser.h"
#include "parser/sat-expression-scanner.h"

extern FILE *       yyin;
extern sat_var_idx  yy_id_counter;

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
    yyin = fopen(input_file,"r");

    if(yyin == NULL) {
        printf("Error: Could not open input file '%s'\n", input_file);
        return 1;
    }

    // Run the parser.
    yyparse();
    printf("[DONE]\n");

    // We are finished with the input now.
    fclose(yyin);

    if(yy_id_counter == 0) {
        printf("Error: input file '%s' contains no expressions\n", input_file);
        return 1;
    }

    // Print some information.
    printf("> ID Count: \t%d\n", yy_id_counter);

    // Build the implication matrix
    sat_imp_matrix * imp_matrix = sat_new_imp_matrix(yy_id_counter);

    // Make sure the empty matrix is consistant first.
    sat_consistancy_check * result = sat_check_imp_matrix(imp_matrix,1);

    if(! result -> is_consistant) {
        printf("Error: empty matrix not consistant. Internal bug.\n");
        free(result);
        return 1;
    }
    free(result);

    // Add all of the assignments to the implication matrix.
    sat_assignment * walker = yy_assignments;
    while(walker != NULL) {
        sat_add_assignment_to_imp_matrix(imp_matrix,walker);
        walker = walker -> next;
    }

    // Free the implication matrix
    sat_free_imp_matrix(imp_matrix);


    return 0;
}

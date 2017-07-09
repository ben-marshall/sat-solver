
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "satsolver.h"

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
    sat_clear_imp_matrix(imp_mat);


    // Free the implication matrix and return.
    sat_free_imp_matrix(imp_mat);

    return 0;
}

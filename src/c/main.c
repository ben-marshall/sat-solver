
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
  
    struct timeval timstr;  //<! structure to hold elapsed time
    double t_start, t_end;  //<! used to calculate elapsed wallclock time
    double time_parse;      //<! Time taken parsing expressions
    double time_alloc;      //<! Time taken allocating matrix
    double time_insert;     //<! Time taken inserting implications.
    double time_prop;       //<! Time taken propagating implications
    double time_check;      //<! Time taken checking consistancy
    double time_domain;     //<! Time taken updating variable domains.
    double time_total;      //<! Total computation time.

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
    
    gettimeofday(&timstr,NULL);
    t_start=timstr.tv_sec+(timstr.tv_usec/1000000.0);

    // Run the parser.
    yyparse();
    printf("[DONE]\n");

    // We are finished with the input now.
    fclose(yyin);
    yylex_destroy();

    gettimeofday(&timstr,NULL);
    t_end=timstr.tv_sec+(timstr.tv_usec/1000000.0);
    time_parse = t_end - t_start;

    // How many variables are there?
    unsigned int variable_count = sat_get_variable_count();
    printf("Total Variables: %d\n", variable_count);
    
    gettimeofday(&timstr,NULL);
    t_start=timstr.tv_sec+(timstr.tv_usec/1000000.0);

    // Build the implication matrix
    sat_imp_matrix * imp_matrix = sat_new_imp_matrix(variable_count);
    
    gettimeofday(&timstr,NULL);
    t_end=timstr.tv_sec+(timstr.tv_usec/1000000.0);
    time_alloc = t_end - t_start;

    
    // Add all of the assignments to the implication matrix.
    gettimeofday(&timstr,NULL);
    t_start=timstr.tv_sec+(timstr.tv_usec/1000000.0);

    printf("Adding assignments to matrix... "); fflush(stdout);

    sat_assignment * walker = yy_assignments;
    while(walker != NULL) {
        sat_add_assignment_to_imp_matrix(imp_matrix,walker);
        walker = walker -> next;
    }
    
    gettimeofday(&timstr,NULL);
    t_end=timstr.tv_sec+(timstr.tv_usec/1000000.0);
    time_insert = t_end - t_start;
    

    printf("[DONE]\n");
    printf("Propagating Implications...     "); fflush(stdout);
    
    gettimeofday(&timstr,NULL);
    t_start=timstr.tv_sec+(timstr.tv_usec/1000000.0);
    
    // Propagate all implications from leaf inputs to roots of assignments.
    sat_propagate_imp_matrix(imp_matrix);
    
    gettimeofday(&timstr,NULL);
    t_end=timstr.tv_sec+(timstr.tv_usec/1000000.0);
    time_prop = t_end - t_start;

    printf("[DONE]\n");
    printf("Checking Consistancy...         "); fflush(stdout);
    
    // Check there are no obvious contradictions and that the matrix is
    // consistant.
    gettimeofday(&timstr,NULL);
    t_start=timstr.tv_sec+(timstr.tv_usec/1000000.0);

    sat_consistancy_check * result = sat_check_imp_matrix(imp_matrix,1);
    
    gettimeofday(&timstr,NULL);
    t_end=timstr.tv_sec+(timstr.tv_usec/1000000.0);
    time_check = t_end - t_start;
    
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
    gettimeofday(&timstr,NULL);
    t_start=timstr.tv_sec+(timstr.tv_usec/1000000.0);

    t_sat_bool sat_result = sat_solve(imp_matrix, 0);

    printf("SAT Solve Result: %d\n", sat_result);

    sat_var_idx i = 0;
    for(i = 0; i < imp_matrix -> variable_count; i ++) {
        sat_expression_variable * v = sat_get_variable_from_id(i);
        sat_check_expectations(v,imp_matrix,SAT_TRUE);
        printf("%d - { %d %d } - %s\n",i, imp_matrix->d_0[i],
            imp_matrix->d_1[i], v -> name);
    }
    
    gettimeofday(&timstr,NULL);
    t_end=timstr.tv_sec+(timstr.tv_usec/1000000.0);
    time_domain = t_end - t_start;
    printf("[DONE]\n");


    // ---- End of program. Clean up. --------

    time_total = time_parse  + time_alloc + 
                 time_insert + time_prop  + time_check;
  
    printf("Perf - Parsing           %.6lf s\n", time_parse);
    printf("Perf - Allocation        %.6lf s\n", time_alloc);
    printf("Perf - Insertion         %.6lf s\n", time_insert);
    printf("Perf - Propagation Time  %.6lf s\n", time_prop);
    printf("Perf - Propagation Count %d\n", imp_matrix->propagation_count);
    printf("Perf - Propagation Rate  %.6lf p/s\n", (double)imp_matrix->propagation_count/time_prop);
    printf("Perf - Checking          %.6lf s\n", time_check);
    printf("Perf - Domain Update     %.6lf s\n", time_domain);
    printf("Perf - Total Time        %.6lf s\n", time_total);

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

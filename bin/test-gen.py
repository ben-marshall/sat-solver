#!/usr/bin/python3

import os
import sys
import random
import subprocess
import time
import argparse

operators = ["&","|","^","~&","~|","~^"]
op_weights= [ 1 , 1 , 1 , 1 , 1 , 1    ]

FORCE_NONE  = 0
FORCE_TRUE  = 1
FORCE_FALSE = 2

class TestVariableAssignment(object):
    
    def __init__(self, assigne, source_variables):
        """
        Create a new assignment expression for the assignee variable.
        The expression can contain any of the variables in the source_variable
        set.
        """
        assert (type(assigne) == TestVariable)
        assert (len(source_variables) > 0)

        self.assigne    = assigne
        self.input_count= min(random.randint(2,10),len(source_variables))
        self.inputs     = random.sample(source_variables,self.input_count)
        
        self.expression = []
        self.exp_ops    = []

        self.__gen_expression__()


    def __gen_expression__(self):
        """
        Generate the expression from the input variables.
        """
        for i in self.inputs:
            self.expression.append(i)
            
            if(len(self.expression) < len(self.inputs)):
                self.exp_ops.append(random.sample(operators,1)[0])

    def __str__(self):
        """
        Return a string representation of the assignment.
        """
        
        tr = "%s = "  % self.assigne.name

        for i in range(0,len(self.exp_ops)):
            tr = tr + ("%s %s " % (self.expression[i].name,self.exp_ops[i]))
        
        tr = tr + ("%s" % self.expression[i].name)

        return tr



class TestVariable(object):
    
    def __init__(self, name,
                       is_input = False,
                       is_output = False):
        """
        Create a new variable with the given string name.
        """
        assert ((is_input and is_output) == False)

        self.name = name
        
        self.force_val = FORCE_NONE

        self.assignment = None

        if(is_input):
            self.force_val = random.sample([FORCE_NONE,FORCE_NONE,
                FORCE_TRUE,FORCE_FALSE],1)[0]

    def get_constraints():
        """
        Return the constraints on the value as a string.
        """
        tr = ""
        if(self.force_0== False):
            tr += "%s == 0\n" % self.name
        if(self.force_1== False):
            tr += "%s == 1\n" % self.name
        return tr


class TestCase(object):
    """
    A class for generating input test cases to the SAT solver
    """
    
    def __init__(self, seed, 
                       iao_ratios=[1,1,1], 
                       min_variables = 10,
                       max_variables = 100):
        """
        Generates a new test case with a given seed.
        """
        self.seed = seed
        random.seed(a = self.seed) 

        self.iao_ratios    = iao_ratios
        self.min_variables = min_variables
        self.max_variables = max_variables
        
        # Never get assigned to
        self.input_variables = set([])

        # Are assigned to, and can be used in other expressions
        self.assign_variables= set([])

        # Are assigned to but are never used
        self.output_variables= set([])

        self.__gen_variables__()
        self.__gen_assignments__()

        self.all_vars = self.input_variables.union(self.output_variables,
                                                   self.assign_variables)


    def __gen_variables__(self):
        """
        Generates the set of variables for the test case according to
        the ratios in self.iao_ratios
        """
        ratio_set = []
        ratio_set += ["i" for i in range(0,self.iao_ratios[0])]
        ratio_set += ["a" for i in range(0,self.iao_ratios[1])]
        ratio_set += ["o" for i in range(0,self.iao_ratios[2])]

        num_vars = random.randint(self.min_variables, self.max_variables)

        for i in range(0, num_vars):

            vt = random.sample(ratio_set,1)[0]

            name = "%s_%d" % (vt,i)
            v = TestVariable(name, is_input = (vt=="i"),is_output=(vt=="o"))

            if(vt == "i"):
                self.input_variables.add(v)
            elif(vt == "a"):
                self.assign_variables.add(v)
            elif(vt == "o"):
                self.output_variables.add(v)

    def __gen_assignments__(self):
        """
        For all non-input variables, generate an assignment expression
        for them.
        """

        for var in self.assign_variables:
            var.assignment = TestVariableAssignment(var,self.input_variables)

        in_and_ass = self.input_variables.union(self.assign_variables)

        for var in self.output_variables:
            var.assignment = TestVariableAssignment(var,in_and_ass)


    def toFile(self, path):
        """
        Write the test case to a file.
        """
        
        with open(path,"w") as fh:
            
            fh.write("\n")

            for var in self.assign_variables:
                a = str(var.assignment)
                fh.write(a+"\n")
            
            fh.write("\n")

            for var in self.output_variables:
                a = str(var.assignment)
                fh.write(a+"\n")

            fh.write("\n")

            for var in self.all_vars:
                if(var.force_val == FORCE_TRUE):
                    fh.write("%s == 1\n" % var.name)
                elif(var.force_val == FORCE_FALSE):
                    fh.write("%s == 0\n" % var.name)

            fh.write("\nend")


def parse_arguments():
    """
    Parses all command line arguments to the program.
    """
    parser = argparse.ArgumentParser(description=__doc__)
    
    parser.add_argument("--output-dir",help="Folder to put test files in.",
        default="./build/test_vectors")

    args = parser.parse_args()
    return args


def main():
    """
    Main program for the test harness.
    """
    
    args = parse_arguments()
    vector_folder = args.output_dir

    if(not os.path.exists(vector_folder)):
        os.makedirs(vector_folder)

    print("Creating test vectors...")

    for i in range(0,50): 
        filename = "%s/tv_%03d.txt" % (vector_folder,i)
        
        seed = time.time()

        tv = TestCase(seed,
            max_variables = 1000,
            iao_ratios    = [1, 5, 1])
        tv.toFile(filename)
        
        print("Test: %s, Seed: %f" % (filename, seed,))
    
    print("Creating test vectors... [DONE]")


if(__name__=="__main__"):
    main()
else:
    pass

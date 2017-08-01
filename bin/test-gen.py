#!/usr/bin/python3

import os
import sys
import random
import subprocess
import time

operators = ["&","|","^","~&","~|","~^"]

class TestVariable(object):
    
    def __init__(self, name,
                       is_input = False,
                       is_output = False):
        """
        Create a new variable with the given string name.
        """
        assert ((is_input and is_output) == False)

        self.name = name
        
        self.force_0 = False
        self.force_1 = False

        if(is_input):
            self.force_0 = random.sample([True,False],1)[0]
            self.force_1 = random.sample([True,False],1)[0]

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



def parse_arguments():
    """
    Parses all command line arguments to the program.
    """
    parser = argparse.ArgumentParser(description=__doc__)
    
    parer.add_argument("--output-dir",help="Folder to put generated files in.",
        default="./work/test_vectors")

    args = parser.parse_args()
    return args


def main():
    """
    Main program for the test harness.
    """
    
    args = parse_arguments()
    vector_folder = args.output_dir

    if(not os.path.exists(vector_folder)):
        os.mkdir(vector_folder)

    print("Creating test vectors...")

    for i in range(0,50): 
        filename = "%s/tv_%03d.txt" % (vector_folder,i)
        
        seed = time.time()

        tv = TestCase(seed)
        tv.generate()
        tv.writeToFile(filename)
        
        print("Test: %s, Seed: %f, Variables: %d, Assignments: %d" %
            (filename, seed,tv.num_variables,len(tv.assignments)))
    
    print("Creating test vectors... [DONE]")


if(__name__=="__main__"):
    main()
else:
    pass

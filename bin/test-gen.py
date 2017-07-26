#!/usr/bin/python3

import os
import sys
import random
import subprocess
import time

operators = ["&","|","^","~&","~|","~^"]

class TestCase(object):
    
    def __init__(self, seed):
        """
        Generates a new test case with a given seed.
        """
        self.seed = seed
        random.seed(a = self.seed) 

        self.num_variables = None
        self.num_constraints = None
        self.num_expectations = None

        self.min_variables = 4
        self.max_variables = 1000

        self.min_constraints = 0
        self.max_constraints = int(self.max_variables / 2)
        
        self.min_expectations = 0
        self.max_expectations = 100000

        self.unassigned_variables = set([])
        self.variables = set([])
        self.assignments= set([])
        self.expressions = set([])
        self.expectations = set([])
        self.constraints = set([])

    def gen_assignment(self, assignee, variables):
        """
        Generate an assignment expression using the supplied variables.
        """
        ass = "%s = " % assignee
        
        first = True

        for i in variables:
            if(first) :
                ass += "%s" % i
                first=False
            else:
                ass += " %s %s " % (random.sample(operators,1)[0], i)
        
        return ass

    def generate(self):
        """
        Create the new test case
        """
        self.num_variables = random.randrange(self.min_variables,
                                              self.max_variables)
        self.max_constraints = int(max(1,self.num_variables / 2))
        self.max_expectations = int(self.num_variables / 2)

        self.num_constraints = random.randrange(self.min_constraints,
                                                self.max_constraints)
        self.num_expectations= random.randrange(self.min_expectations,
                                                self.max_expectations)

        for i in range(0, self.num_variables):
            self.variables.add(hex(i)[1:])
            self.unassigned_variables.add(hex(i)[1:])

        for i in range(0, int(self.num_variables/3*2)):
            varcount    = random.randint(1, min(10,len(self.unassigned_variables)))
            assignee    = random.sample(self.unassigned_variables,1)[0]
            self.unassigned_variables.remove(assignee)
            varset      = random.sample(self.variables, varcount)

            ass = self.gen_assignment(assignee, varset)
            self.assignments.add(ass)



    def writeToFile(self, path):
        """
        Write the new test case to file.
        """

        with open(path,"w") as fh:
            for a in self.assignments:
                fh.write("%s\n" % a)

            fh.write("\nend\n\n")
        

def main():
    """
    Main program for the test harness.
    """

    vector_folder = "./build/test_vectors"

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

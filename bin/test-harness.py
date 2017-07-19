#!/usr/bin/python3

import os
import sys
import random
import subprocess

def new_test_vector(save_to):
    """
    Create a new test vector and save it to the supplied file path.
    """

    variable_set = ["v_%d" % i for i in range(2,random.randint(10,50))]
    num_variables= len(variable_set)
    stop = int((2+(num_variables ** 2))/4)
    num_relations= random.randint(2, stop)

    ops = ["|", "&","~"]

    ta = []

    for i in range(0, num_relations):
        
        op          = random.sample(ops, 1)[0]
        assignee    = random.sample(variable_set, 1)[0]
        var_lhs     = random.sample(variable_set, 1)[0]
        var_rhs     = random.sample(variable_set, 1)[0]

        if (op == "~"):
            ta.append("%s = %s %s"      % (assignee, op, var_rhs))
        else:
            ta.append("%s = %s %s %s"   % (assignee, var_lhs,op, var_rhs))

    with open(save_to,"w") as fh:
        for c in ta:
            fh.write(c+"\n")
        fh.write("end\n")

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
        new_test_vector(filename)
    
    print("Creating test vectors... [DONE]")


if(__name__=="__main__"):
    main()
else:
    pass

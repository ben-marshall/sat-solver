#!/usr/bin/python3

import os
import sys
import random
import subprocess

def main():
    """
    Main program for the test harness.
    """

    variable_set = range(2,random.randint(10,1000))
    num_variables= len(variable_set)
    num_relations= random.randint(1, (num_variables ** 2)/2)

    print("Variables: %d" % num_variables)
    print("Relations: %d" % num_relations)

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

    for c in ta:
        print(c)
    print("END")


if(__name__=="__main__"):
    main()
else:
    pass

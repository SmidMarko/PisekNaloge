"""
Using an external generator for the inputs
"""
from generators import *
from random import *
 
generator = ExternalCppGenerator()
generator.setFile("generate.cpp")

for nb in range(1, 10):
    generator.addExternalTest("big-{:09d}".format(nb),
                              "{} integers".format(nb),
                              str(nb))
 
generator.runSolution("sol-reference.cpp")

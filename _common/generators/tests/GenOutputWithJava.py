"""
Basic example with input provided and Java solution
"""
from generators import *

generator = Generator()
generator.addTest("example", "Normal", 
"""
3
""")
generator.addTest("example", "Special case", 
"""
0
""")

generator.runSolution("sol-reference.java")

"""
Example with random generation
"""
from generators import *
from random import *

class MyGenerator(Generator):
    def generate(self, name, comment):
        nb = randint(-1000, 1000)
        cin = "{}\n".format(nb)
        self.addTest(name, comment, cin)

generator = MyGenerator()
generator.addTest("example", "Normal", 
"""
3
""")
generator.addTest("example", "Special case", 
"""
0
""")
for loop in range(10):
    generator.generate("random", "Random case")

generator.runSolution("sol-reference.py")

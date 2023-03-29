"""
Basic example with both input and output provided
"""
from generators import *

generator = Generator()
generator.addTest("example", "Normal", 
"""
3
""",
"""
9
""")

generator.addTest("example", "Special case", 
"""
0
""",
"""
0
""")

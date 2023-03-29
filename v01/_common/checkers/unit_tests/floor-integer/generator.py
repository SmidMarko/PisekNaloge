from UnitTestGenerator import *

th_good = 1e-5 / 10
th_bad = 1e-5 * 10
generator = UnitTestGenerator()
# cin, cout, candidate, success

# Normal : lower half
generator.add("10.3","10.3","10.0", True)    # Good
generator.add("10.3","10.3","9.0", False)    # Too low
generator.add("10.3","10.3","11.0", False)   # Too high
# Normal : higher half
generator.add("10.8","10.8","10.0", True)    # Good
generator.add("10.8","10.8","9.0", False)    # Too low
generator.add("10.8","10.8","11.0", False)   # Too high
# Normal : middle
generator.add("10.5","10.5","10.0", True)    # Good
generator.add("10.5","10.5","9.0", False)    # Too low
generator.add("10.5","10.5","11.0", False)   # Too high

# Exact
generator.add("10.0","10.0","10.0", True)    # Good
generator.add("10.0","10.0","9.0", True)     # Good also
generator.add("10.0","10.0","8.0", False)    # Too low
generator.add("10.0","10.0","11.0", False)   # Too high

# Exact + epsilon
generator.add(str(10.0+th_good),str(10.0+th_good),"10.0", True)    # Good
generator.add(str(10.0+th_good),str(10.0+th_good),"9.0", True)     # Good also
generator.add(str(10.0+th_bad),str(10.0+th_bad),"9.0", False)       # But not this
generator.add(str(10.0+th_good),str(10.0+th_good),"8.0", False)    # Too low
generator.add(str(10.0+th_good),str(10.0+th_good),"11.0", False)   # Too high

# Exact - epsilon
generator.add(str(10.0-th_good),str(10.0-th_good),"9.0", True)       # Good
generator.add(str(10.0-th_good),str(10.0-th_good),"10.0", True)      # Good also
generator.add(str(10.0-th_bad),str(10.0-th_bad),"10.0", False)       # But not this
generator.add(str(10.0-th_good),str(10.0-th_good),"8.0", False)      # Too low
generator.add(str(10.0-th_good),str(10.0-th_good),"11.0", False)     # Too high

generator.finished()


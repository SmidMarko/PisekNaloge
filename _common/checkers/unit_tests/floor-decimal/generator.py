from UnitTestGenerator import *

th_good = 1e-5 / 10  /100
th_bad = 1e-5 * 10   /100

generator = UnitTestGenerator()
# cin, cout, candidate, success

# Normal : lower half
generator.add("10.423","10.423","10.42", True)    # Good
generator.add("10.423","10.423","10.41", False)    # Too low
generator.add("10.423","10.423","10.43", False)   # Too high
# Normal : higher half
generator.add("10.428","10.428","10.42", True)    # Good
generator.add("10.428","10.428","10.41", False)    # Too low
generator.add("10.428","10.428","10.43", False)   # Too high
# Normal : middle
generator.add("10.425","10.425","10.42", True)    # Good
generator.add("10.425","10.425","10.41", False)    # Too low
generator.add("10.425","10.425","10.43", False)   # Too high

# Exact
generator.add("10.42","10.42","10.42", True)    # Good
generator.add("10.42","10.42","10.41", True)     # Good also
generator.add("10.42","10.42","10.40", False)    # Too low
generator.add("10.42","10.42","10.43", False)   # Too high

# Exact + epsilon
generator.add(str(10.42+th_good),str(10.42+th_good),"10.42", True)    # Good
generator.add(str(10.42+th_good),str(10.42+th_good),"10.41", True)     # Good also
generator.add(str(10.42+th_bad),str(10.42+th_bad),"10.41", False)       # But not this
generator.add(str(10.42+th_good),str(10.42+th_good),"10.40", False)    # Too low
generator.add(str(10.42+th_good),str(10.42+th_good),"10.43", False)   # Too high

# Exact - epsilon
generator.add(str(10.42-th_good),str(10.42-th_good),"10.41", True)       # Good
generator.add(str(10.42-th_good),str(10.42-th_good),"10.42", True)      # Good also
generator.add(str(10.42-th_bad),str(10.42-th_bad),"10.42", False)       # But not this
generator.add(str(10.42-th_good),str(10.42-th_good),"10.40", False)      # Too low
generator.add(str(10.42-th_good),str(10.42-th_good),"10.43", False)     # Too high

generator.finished()


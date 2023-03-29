#!/usr/bin/env python3
import logging, sys
from optparse import OptionParser
from TfmServer import *

## Configuration
usage = """
tfm gen all
tfm gen tests
tfm gen sani
tfm gen check
"""

parser = OptionParser(usage=usage, version="%prog 1.0", epilog="Don't hesitate to propose improvments.")
parser.add_option("-l", "--log", default="INFO", choices=["DEBUG", "INFO", "WARNING", "ERROR"],
                  help="log level: DEBUG, INFO, WARNING or ERROR [default:%default]",
                  dest="logLevel", metavar="LEVEL")
parser.add_option("-c", "--common", action="store", type="string",
                  help="common dir",
                  dest="commonDir", metavar="COMMON")

(options, args) = parser.parse_args()

if options.commonDir == None:
   options.commonDir = input("'_common' directory to use ? ")
   
TfmCommon.setCommonDir(path.realpath(options.commonDir))


## Apply options
if len(args) < 1:
   parser.error("incorrect number of arguments")

numeric_level = getattr(logging, options.logLevel.upper(), None)
logging.getLogger().setLevel(numeric_level)


#print(options)
#print(args)

def taskPath(args, pos):
   path = "."
   if len(args) > pos:
      path = args[pos]
   path = os.path.realpath(path)
   while os.path.basename(path)[0].islower():
      (oldpath, path) = (path, os.path.realpath(path + "/.."))
   return oldpath

## Main command
def main():
 
   cmd = args[0]
   if cmd == "gen":
      subCmd = args[1] if len(args) > 1 else ""
      if subCmd == "tests":
         task = LocalTask(taskPath(args, 2))
         task.generateTests()
      elif subCmd == "sani":
         task = LocalTask(taskPath(args, 2))
         task.sanitizer.compile()
      elif subCmd == "check":
         task = LocalTask(taskPath(args, 2))
         task.checker.compile()
      elif subCmd == "all":
         task = LocalTask(taskPath(args, 2))
         task.generateTests()
         task.sanitizer.compile()
         task.checker.compile()
      else:
         parser.print_help()
   #elif cmd == "clean":
   #   task = LocalTask(taskPath(args, 1))
   #   task.clean()
   #elif cmd == "run":
   #   subCmd = args[1]
   #   if subCmd == "sani":
   #      task = Sanitizer(taskPath(args, 2))
   #      task.run()
   #   else:
   #      parser.print_help()
   elif cmd == "test":
      sol = Solution(args[1])
      sol.run()
   else:
      parser.print_help()

if __name__ == "__main__":
   main()


import textwrap
import logging
import subprocess
import signal, sys
import os
from os import path



def signal_handler(signal, frame):
   print("\n" + path.basename(sys.argv[0]) + ' has been interrupted !')
   sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)

# Redefine it if not defined
if not "indent" in dir(textwrap):
   textwrap.indent = lambda text, prefix: "\n".join(map(lambda x:prefix + x, text.split("\n")))

## Configuring logging module
# Configure format
logging.basicConfig(format='%(asctime)s  %(levelname)s: %(message)s', level=logging.DEBUG, datefmt='%I:%M:%S')
# Dirty solution to add color to the output
logging.addLevelName( logging.WARNING, "\033[1;31m%s\033[1;m" % logging.getLevelName(logging.WARNING))
logging.addLevelName( logging.ERROR, "\033[1;41m%s\033[1;m" % logging.getLevelName(logging.ERROR))


"""
Useful decorator to add memoization
"""
def memorize(function):
  memo = {}
  def wrapper(*args):
    if args in memo:
      return memo[args]
    else:
      rv = function(*args)
      memo[args] = rv
      return rv
  return wrapper


"""
Main class for the "Test File Management"
"""
class TfmCommon:
   ### Data
   # The 'common' directory with all the libraries
   commonDir = None
  
   ### "public" functions
   ## Configuration
   def setCommonDir(commonDir):
      TfmCommon.commonDir = commonDir

   ## Actions
   """
   Display all the parameters
   """
   def showParameters():
      print("Parameters:")
      print("* Common directory:\n{}".format(TfmCommon.commonDir))

   ### "internal functions"
   """
   Test the validity of the paramaters
   """
   def testValidity():
      def internalTest():
         if TfmCommon.commonDir == None:
            logging.error("No common directory defined in 'TfmCommon'")
            return False
         return True
      if not internalTest():
         exit(1)
      return True



"""
Executes a command.
Log the command and display warning if error.
"""
def executeCommand(cmd):
   logging.debug("Executing command:\n%s", textwrap.indent(cmd, "\t"))
   status, output = subprocess.getstatusoutput(cmd)
   if status != 0:
      logging.warning("Return code is %s for command\n%s", status, textwrap.indent(cmd, "\t"))
      logging.debug("Output:\n%s", textwrap.indent(output, "\t"))
   return status, output

"""
Executes a command and exit if returns code not equal to 0.
"""
def exitIfNot0(cmd, *args):
   status, output = executeCommand(cmd)
   if status != 0:
      args = list(args) + [textwrap.indent(output, "\t")]
      logging.error(args[0] + "\n%s", *args[1:])
      exit(1)
   return output

"""
Implementation the remove the trailing "/".
"""
def myBasename(fileName):
   if fileName[-1] == "/":
      fileName = fileName[0:-1]
   return path.basename(fileName)

if __name__ == "__main__":
    print("Everything ok for 'TfmCommon'")

import re
from os import path
from TfmCommon import *
from glob import glob


class Checker():
   # Some paramaters
   FILE_NAME = "check.cpp"

   # Task data
   taskDir = None
   newMode = False

   """
   Constructor
   taskDir : a real existing local directory
   """
   def __init__(self, taskDir):
      self.newMode = os.path.exists(taskDir + "/tests")
      self.taskDir = path.realpath(taskDir)
      if not path.isdir(self.taskDir):
         logging.error("The directory '%s' does not exists", self.taskDir)
         exit(1)

   """
   Compile the checker
   """
   def compile(self):   
      if self.newMode:
         srcDir = self.taskDir + "/tests/gen/"
         dstDir = self.taskDir + "/tests/files/"
         checkerName = srcDir + "/" + self.FILE_NAME
      else:
         srcDir = self.taskDir
         dstDir = self.taskDir
         checkerName = srcDir + "/" + self.FILE_NAME
      logging.info("%s : compiling checker", self.taskDir)
      if path.exists(checkerName):
         cmd = "cd {SRC_DIR} && mkdir -p {DST_DIR} && g++ -O2 -W -Wall -Wno-unused-result -I{FIOI_COMMON}/checkers -o {DST_DIR}/check {NAME}".format(
            FIOI_COMMON = TfmCommon.commonDir, NAME = self.FILE_NAME, SRC_DIR = srcDir, DST_DIR = dstDir)
         exitIfNot0(cmd, "Compiling the checker for '%s'", self.taskDir)
      else:
         logging.info("No checker found in {}".format(srcDir))
      return True

   def clean(self):
      logging.info("%s : cleaning checker", self.taskDir)
      if path.exists(self.taskDir + "/check"):
         os.remove(self.taskDir + "/check")
      if path.exists(self.taskDir + "/tests/files/check"):
         os.remove(self.taskDir + "/tests/files/check")

class Sanitizer():
   # Some paramaters
   FILE_NAME = "sanitizer.cpp"

   # Task data
   taskDir = None
   newMode = False

   """
   Constructor
   taskDir : a real existing local directory
   """
   def __init__(self, taskDir):
      self.newMode = os.path.exists(taskDir + "/tests")
      self.taskDir = path.realpath(taskDir)
      if not path.isdir(self.taskDir):
         logging.error("The directory '%s' does not exists", self.taskDir)
         exit(1)

   """
   Compile the sanitizer
   """
   def compile(self):
      if self.newMode:
         srcDir = self.taskDir + "/tests/gen/"
         dstDir = self.taskDir + "/tests/files/"
         sanitizerName = srcDir + "/" + self.FILE_NAME
      else:
         srcDir = self.taskDir
         dstDir = self.taskDir
         sanitizerName = srcDir + "/" + self.FILE_NAME

      logging.info("%s : compiling sanitizer", self.taskDir)
      if path.exists(sanitizerName):
         cmd = "cd {SRC_DIR} && mkdir -p {DST_DIR} && g++ -std=c++0x -O2 -W -Wall -Wno-unused-result -I{FIOI_COMMON}/sanitizer -o {DST_DIR}/sanitizer {NAME}".format(
            FIOI_COMMON = TfmCommon.commonDir, SRC_DIR = srcDir, DST_DIR = dstDir, NAME = self.FILE_NAME)
         exitIfNot0(cmd, "Compiling the sanitizer for '%s'\n%s", self.taskDir)            
      return True

   def clean(self):
      logging.info("%s : cleaning sanitizer", self.taskDir)
      if path.exists(self.taskDir + "/sanitizer"):
         os.remove(self.taskDir + "/sanitizer")
      if path.exists(self.taskDir + "/tests/files/sanitizer"):
         os.remove(self.taskDir + "/tests/files/sanitizer")

   """
   Run the sanitizer
   """
   def run(self):
      print("runSanitizer : NOT READY")
      exit(0)
      logging.info("%s : running sanitizer", self.taskDir)
      if self.compile():
         for inFile in glob(self.taskDir + "/*.in"):
            logging.debug("Checking {}".format(inFile))
            cmd = "{}/sanitizer < {}".format(self.taskDir, inFile)
            exitIfNot0(cmd, "Runing the sanitizer for %s", self.taskDir)
      return True

"""
Class to manage a local task
"""
class LocalTask():
   # Some paramaters
   file_genPython = "generator.py"
   file_genScript = "gen.sh"

   # Task data
   taskDir = None
   newMode = False

   checker = None
   sanitizer = None
   """
   Constructor
   taskDir : a real existing local directory
   """
   def __init__(self, taskDir):
      self.newMode = os.path.exists(taskDir + "/tests")
      self.taskDir = path.realpath(taskDir)
      if not path.isdir(self.taskDir):
         logging.error("The directory '%s' does not exists", self.taskDir)
         exit(1)

      self.checker = Checker(self.taskDir)
      self.sanitizer = Sanitizer(self.taskDir)

   """
   Generate all the test files
   """
   def generateTests(self):
      if self.newMode:
         srcDir = self.taskDir + "/tests/gen/"
         geneName = self.taskDir + "/tests/gen/" + self.file_genPython
      else:
         srcDir = self.taskDir
         geneName = self.taskDir + "/" + self.file_genPython
      logging.info("%s : generating tests", self.taskDir)
      
      # Python generator
      if path.exists(geneName):
         cmd = "cd {SRC_DIR} && export PYTHONPATH='{FIOI_COMMON}/generators/' && python3 -B '{GEN}' .".format(
            FIOI_COMMON = TfmCommon.commonDir, GEN = self.file_genPython, SRC_DIR = srcDir)
         output = exitIfNot0(cmd, "Executing the generator for %s", self.taskDir)
         logging.debug("Generator output:\n%s", textwrap.indent(output, "\t"))
      # gen.sh script
      elif path.exists(srcDir + "/" + self.file_genScript):
         cmd = "cd {} && bash {}".format(srcDir, self.file_genScript)
         exitIfNot0(cmd, "Executing gen.sh for %s", self.taskDir)
      if path.exists(srcDir + "/" + "genLib.py"):
         cmd = "cd {SRC_DIR} && export PYTHONPATH='{FIOI_COMMON}/generators/' && python3 -B '{GEN}' .".format(
            FIOI_COMMON = TfmCommon.commonDir, GEN = "genLib.py", SRC_DIR = srcDir)
         output = exitIfNot0(cmd, "Executing the lib generator for %s", self.taskDir)
         logging.debug("Generator output:\n%s", textwrap.indent(output, "\t"))
      return True
     




   """
   Clean the task of all generated/compiled files
   """
   def clean(self):
      print("clean : NOT READY")
      exit(0)
      logging.info("%s : cleaning", self.taskDir)
      # Remove generated test files
      for ext in [".in", ".out"]:
         for File in sorted(glob(self.taskDir + "/gen-*" + ext)):
            os.remove(File)
      # Remove executables
      for File in glob(self.taskDir + "/*"):
         if os.access(File, os.X_OK):
            os.remove(File)

   """
   Useful functions
   """
   def isEmpty(self):
      return len(glob(self.taskDir + "/*")) == 0

   def hasTests(self):
      return len(glob(self.taskDir + "/*.in")) != 0

   def listBadTests(self):
      allTests = glob(self.taskDir + "/*.in") + glob(self.taskDir + "/*.out")
      return list(filter(lambda x:not re.search("^[^\.]{1,}\.[0-9]{2,}\.(in|out)$", x), allTests))

def taskPath(path):
   path = os.path.realpath(path)
   while os.path.basename(path)[0].islower():
      (oldpath, path) = (path, os.path.realpath(path + "/.."))
   return oldpath


class Solution:
   # Task data
   taskDir = None
   solFile = None

   """
   Constructor
   solFile : a solution file
   """
   def __init__(self, solFile):
      self.solFile = solFile
      self.taskDir = taskPath(solFile)
      if not path.exists(self.solFile):
         logging.error("The file '%s' does not exists", self.solFile)
         exit(1)

   """
   Compile the solution
   """
   def compile(self):
      srcDir = self.taskDir + "/tests/gen/"
      dstDir = "/tmp/solTfm/"

      logging.info("%s : compiling solution '%s'", self.taskDir, self.solFile)
      exitIfNot0("rm -rf {DST_DIR}; mkdir {DST_DIR}".format(DST_DIR = dstDir), "Copying lib files")

      def copyLibFiles(src, dst):
         if os.path.exists(src):
            cmd = "cp {SRC}/* {DST}".format(SRC = src, DST = dst)
            exitIfNot0(cmd, "Copying lib files")



      if path.exists(self.solFile):
         _, ext = os.path.splitext(self.solFile)
         cmd = ""
         # regler les includes et tout ça, un poil complexe pour bien le faire comme il faut....
         # plus tard
         filesDir = self.taskDir + "/tests/files/"
         if ext == ".cpp":
            copyLibFiles(TfmCommon.commonDir + "/lib/c", dstDir)
            copyLibFiles(filesDir + "/lib/c", dstDir)
            
            cmd = "g++ -W -Wall -O2 -o {DST_DIR}/sol {SOL_NAME}".format(DST_DIR = dstDir, SOL_NAME = self.solFile)
            exitIfNot0(cmd, "Compiling the solution '%s'", self.solFile)
         elif ext == ".py":
            copyLibFiles(TfmCommon.commonDir + "/lib/python", dstDir)
            copyLibFiles(filesDir + "/run/", dstDir)

            cmd = "(echo '#! /bin/bash'; echo 'python3 {SOL_NAME}') > {DST_DIR}/sol; chmod 755 {DST_DIR}/sol; cp {SOL_NAME} {DST_DIR}/".format(
               DST_DIR = dstDir, SOL_NAME = self.solFile)
            exitIfNot0(cmd, "Compiling the solution '%s'", self.solFile)
         else:
            logging.error("Unknown extension '%s' for solution '%s'", ext, self.solFile)
            exit(1)
         
      return True

   """
   Run the solution
   """
   def run(self):
      srcDir = self.taskDir + "/tests/gen/"
      dstDir = "/tmp/solTfm/"
      solFile = "/tmp/solTfm/sol"
      filesDir = self.taskDir + "/tests/files/"


      logging.info("%s : running solution '%s'", self.taskDir, self.solFile)
      if self.compile():
         (nbTotal, nbGood) = (0,0)
         for inFile in glob(self.taskDir + "/tests/files/*.in"):
            logging.debug("Checking {}".format(inFile))
     
            if os.path.exists(filesDir + "/check"):
               cmd = "bash -c 'cd {DST_DIR} && {CHECK} <({SOL} < {inF}) {inF} {outF}'".format(
                  inF = inFile, outF = inFile.replace(".in", ".out"), SOL = solFile, DST_DIR = dstDir, CHECK = filesDir + "/check")
               status, output = executeCommand(cmd)
               isGood = int(output.split("\n")[0]) == 100
            else:
               cmd = "bash -c 'cd {DST_DIR} && diff -q <({SOL} < {inF}) {outF} > /dev/null 2>&1'".format(
                  inF = inFile, outF = inFile.replace(".in", ".out"), SOL = solFile, DST_DIR = dstDir)
               status, output = executeCommand(cmd)   
               isGood = (status == 0) 
 
            logging.debug(output)
            (nbTotal, nbGood) = (nbTotal + 1, nbGood + isGood)
            if not isGood:
               logging.info("Test {} : failure".format(inFile))
         print("Solution {} : {}/{} tests, score of {}/100".format(self.solFile, nbGood, nbTotal, round(nbGood/nbTotal*100)))
         
      return True

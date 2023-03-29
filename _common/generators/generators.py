import os
import sys
import subprocess
import random
import re
import bisect
import string
import tempfile

def error(msg):
   print(msg)
   sys.exit(2)

def openFile(name):
   # Open file for writing
   try:
      d = os.path.dirname(name)
      if not os.path.exists(d):
         os.makedirs(d)
   except OSError:
      error('Impossible to creates the directory : "' + d + '"')
   try:
      f = open(name, 'w')
   except IOError:
      error('Impossible to open the file : "' + name + '"')
   return f

"""
Represents a basic Test
"""
class Test:
   _id = 0
   name = None
   comment = None
   language = None
   inFile = None
   ouFile = None
   def __init__(self, name = None, comment = None):
      self.inc()
      self.setName(name)
      self.setComment(comment)

   def __del__(self):
      for f in [self.inFile, self.ouFile]:
         if f != None:
            f.close

   def setDir(dir):
      Test.dir = dir

   def setName(self, name):
      self.name = name

   def setComment(self, comment):
      self.comment = comment

   def setLanguage(self, language):
      self.language = language

   def getFile(self, dir, ext):
      if self.language == None:
         self.language = ""
      name = os.path.join(dir, self.language, "gen-" + self.name + "." + str(self.getIdFormatted()) + "." + ext)

      if ext == 'in' and self.inFile != None:
         f = self.inFile
      if ext == 'out' and self.ouFile != None:
         f = self.ouFile
      else:
         f = openFile(name)
         if ext == 'in':
            self.inFile = f
         else:
            self.ouFile = f

      return f, name

   def getInputFile(self):
      return self.getFile(Test.dir, "in")

   def getOutputFile(self):
      return self.getFile(Test.dir, "out")


   def inc(self):
      Test._id = Test._id + 1

   def id(self):
      return Test._id

   def getIdFormatted(self):
      return "{:03d}".format(Test._id)


class Generator:
   directory = None
   allTests = []
   saniFile = "sanitizer.cpp"
   saniCompiled = False
   def __init__(self):
      if len(sys.argv) != 2:
         error("Wrong number of arguments")

      genDir = os.path.realpath(sys.argv[1])
      if not os.path.isdir(genDir):
         error("{} is not a directory !".format(genDir))

      directory = os.path.realpath(genDir + "/../files")

      Test.setDir(directory)
      os.chdir(genDir)# TODO : should be remove, everything should be absolute

      # Initialise seed
      random.seed(os.path.basename(os.getcwd()))

   def __del__(self):
      #pass
      self.runSanitizerAll()

   def getPathCommonDir(self):
      m = re.match('.*/_common/',  __file__)
      return m.group(0)


   curTest = None
   def newTest(self, name, comment):
      curTest = Test(name, comment)

   def cleanString(self, str):
      return str.strip() + "\n"

   def msgStartTest(self, name):
      print("Creation of the test {}".format(name))

   def getTestFiles(self, name, comment, language = ""):
      test = Test(name, comment)
      test.setLanguage(language)
      fIn, nameCin = test.getInputFile()
      fOut, nameCout = test.getOutputFile()
      return fIn, fOut, nameCin, nameCout
   
   def setSanitizerFile(self, fileName):
      self.saniFile = fileName

   def runSanitizerAll(self):
      if not(self.saniCompiled) and self.compileSanitizer():
         for (fin, fout, isOutGiven) in self.allTests:
            print("Checking test file {}".format(fin))
            self.runSanitizer(fin)
         self.cleanSanitizer()
         self.saniCompiled = True

   def compileSanitizer(self):
      if not os.path.exists(self.saniFile):
         print("--WARNING : no sanitizer--")
         return False
      cmd = "g++ -W -Wall -std=c++0x -I{commonDir}/sanitizer/ -o sanitizer_check {saniFile}".format(commonDir = self.getPathCommonDir(), saniFile = self.saniFile)
      status, output = subprocess.getstatusoutput(cmd)
      if status != 0:
         print("ERROR, sanitizer {} is not a valid C++ file \n\n {}\n".format(self.saniFile,output))
         exit()
      return True

   def cleanSanitizer(self):
      status, output = subprocess.getstatusoutput("rm -f sanitizer_check")

   def runSanitizer(self, nameFile):
      status, output = subprocess.getstatusoutput("./sanitizer_check < " + nameFile)
      if status != 0:
         print('ERROR, invalid test file {} : \n{}\n'.format(nameFile, output))

   # Tests that depends of the langage
   languages = []  
   language = None
   def setLanguage(self, language = None):
      self.language = language

   def setLanguages(self, languages):
      self.languages = ["all"] + languages

   def addTest(self, name, comment, cin = None, cout = None):
      isOutGiven = True
      if cin == None:
         cin = ""
      if cout == None:
         cout = ""
         isOutGiven = False

      if len(self.languages) == 0:
         possible = [""]
      elif self.language == None:
         possible = self.languages
      else:
         possible = [self.language]

      for language in possible:
         fIn, fOut, nameCin, nameCout = self.getTestFiles(name, comment, language)
         fIn.write(self.cleanString(cin))
         fOut.write(self.cleanString(cout))
         self.allTests.append([nameCin, nameCout, isOutGiven])
      
         self.msgStartTest(nameCin)

   def runSolution(self, name):
      self.runSanitizerAll()

      formatFunction = lambda x : x.format(commonDir = self.getPathCommonDir())
      javaLib = map(formatFunction, [
         '{commonDir}/lib/java/algorea/Scanner.java'
         ])
      jvsLib = map(formatFunction,[
         '{commonDir}/lib/jvs/javascool/Misc.java', 
         '{commonDir}/lib/jvs/javascool/Stdin.java', 
         '{commonDir}/lib/jvs/javascool/Stdout.java'
         ])

      if not os.path.exists(name):
         exit("ERROR, file {} does not exists".format(name));
      
      isRunned = False

      if re.search(".c$", name):
         isRunned = True
         status, output = subprocess.getstatusoutput("gcc -W -Wall -std=gnu99 -O2 -I ../files/lib/c -o __prog "+name+" -lm")
         if status != 0:
            exit('ERROR, file {} is not a valid C file \n\n {}'.format(name,output))

      if re.search(".cpp$", name):
         isRunned = True
         status, output = subprocess.getstatusoutput("g++ -W -Wall -O2 -I ../files/lib/c -o __prog "+name)
         if status != 0:
            exit('ERROR, file {} is not a valid C++ file \n\n {}'.format(name,output))

      if re.search(".java$", name):
         isRunned = True
         status, output = subprocess.getstatusoutput("gcj --main=Main -o __prog "+name+" " + ' '.join(javaLib) )
         if status != 0:
            exit('ERROR, file {} is not a valid Java file \n\n {}'.format(name,output))

      if re.search(".jvs$", name):
         isRunned = True
         if not os.path.exists(formatFunction("{commonDir}/transformer/javascool/FranceIOIJvs2Java")):
            subprocess.getoutput(formatFunction("cd {commonDir}/; bash updateAll.sh"))
         status, output = subprocess.getstatusoutput(formatFunction("{commonDir}/transformer/javascool/FranceIOIJvs2Java "+name+" Main.java"))
         if status != 0:
            exit('ERROR, file {} is not a valid Jvs file \n\n {}'.format(name,output))
         status, output = subprocess.getstatusoutput("gcj --main=Main -o __prog Main.java " + ' '.join(javaLib) + " " + ' '.join(jvsLib))
         if status != 0:
            exit('ERROR, file {} is not a valid Jvs file \n\n {}'.format(name,output))

      if re.search(".py$", name):
         isRunned = True

      if not isRunned:
         exit('ERROR, solution "{}" is not supported\n'.format(name))

      for (fin, fout, isOutGiven) in self.allTests:
         print("Generation of the .out for {}".format(fin))

         # The output was already given
         if isOutGiven:
            # Backup outputfile
            subprocess.getoutput("mv {fout} {fout}.tmp".format(fout=fout))

         # The output needs to be generated
         if re.search(".c$", name):
            subprocess.getoutput("./__prog < "+fin+">"+fout)

         if re.search(".cpp$", name):
            subprocess.getoutput("./__prog < "+fin+">"+fout)

         if re.search(".java$", name):
            subprocess.getoutput("./__prog < "+fin+">"+fout)

         if re.search(".jvs$", name):
            subprocess.getoutput("./__prog < "+fin+">"+fout)

         if re.search(".py$", name):
            error = subprocess.getoutput("python3 "+name+" < "+fin+">"+fout)
            if error != "":
               exit('ERROR, file {} is not a valid Python file \n\n {}'.format(name, error))

         # The output was already given
         if isOutGiven:
            # Analyse the output
            # Case 1 : there is a checker
            if os.path.exists("check.cpp"):
               # Compile checker
               status, output = subprocess.getstatusoutput("g++ -W -Wall -O2 -I{commonDir}/checkers/ -o __check check.cpp".format(commonDir = self.getPathCommonDir()))
               if status != 0:
                  exit('ERROR, file {} is not a valid C++ file \n\n {}'.format("check.cpp",output))
               # Run it
               status, output = subprocess.getstatusoutput("./__check {} {} {}".format(fout+".tmp", fin, fout))
               if status != 0:
                  exit('WARNING, the checker has not validated the provided output \n\n {}'.format(output))

            # Case 2 : there is no checker
            else:
               error = subprocess.getoutput("diff "+fout+".tmp "+fout)
               if error != "":
                  content1 = subprocess.getoutput("cat "+fout+".tmp ")
                  content2 = subprocess.getoutput("cat "+fout)
                  # Case when newline in reference output and nothing in produced one
                  if content1.strip() != "" or content2.strip() != "":
                     print("WARNING, provided file is different of the generated one \n{}".format(error))
            # Use provided outputfile in all cases
            subprocess.getoutput("mv {fout}.tmp {fout}".format(fout=fout))

      subprocess.getoutput("rm -f __prog __check")


class OneTestGenerator(Generator):
   def __init__(self, cin = None, cout = None):
      Generator.__init__(self)
      self.addTest("example", "Unique test", cin, cout)
      


class ExternalCppGenerator(Generator):
   externalFile = None
   def __init__(self, cin = None, cout = None):
      Generator.__init__(self)
      
   def __del__(self):
      subprocess.getoutput("rm generate")
      
   def setFile(self, nameFile):
      self.externalFile = nameFile
      msg = subprocess.getoutput("g++ -W -Wall -O2 -o generate " + self.externalFile)
      if msg != "":
         print("Compilation message : ")
         print(msg)
      
   def addExternalTest(self, name, comment, cin = None):
      if self.externalFile == None:
         exit("Error, no external file specified")
      if cin == None:
         cin = ""

      if len(self.languages) == 0:
         possible = [""]
      elif self.language == None:
         possible = self.languages
      else:
         possible = [self.language]

      for language in possible:
         fIn, fOut, nameCin, nameCout = self.getTestFiles(name, comment, language)
         tmp = tempfile.NamedTemporaryFile(mode = "w", delete = False)
         tmp.write(cin)
         tmp.close();
         msg = subprocess.getoutput("cat " + tmp.name + " | ./generate  > " + nameCin + " && rm -f " + tmp.name)
         self.allTests.append([nameCin, nameCout, False])
      
         self.msgStartTest(nameCin)

class StringGen:
   # Approximated frequency of letters in english
   # http://www.data-compression.com/english.html
   # http://en.wikipedia.org/wiki/Letter_frequency
   englishFreq =\
      [ 0.0651738,0.0124248,0.0217339,0.0349835,0.1041442, # a-e
        0.0197881,0.0158610,0.0492888,0.0558094,0.0009033, # f-j
        0.0050529,0.0331490,0.0202124,0.0564513,0.0596302, # k-o
        0.0137645,0.0008606,0.0497563,0.0515760,0.0729357, # p-t
        0.0225134,0.0082903,0.0171272,0.0013692,0.0145984, # u-y
        0.0007836, # z
        0.1918182,0.07] # space-specials
   # Cumulative frequencies
   def cumsum(it):
       s = sum(it)
       total = 0
       for x in it:
           total += x / s
           yield total
   englishCumFreq = list(cumsum(englishFreq))


   # Generate a random letter
   def genLetter(allowSpaces = False, allowSpecials = False):
      while True:
         pos = bisect.bisect(StringGen.englishCumFreq, random.random())
         if pos == 26 and allowSpaces == False:
            continue
         if pos == 27 and allowSpecials == False:
            continue

         if pos < 26:
            return string.ascii_lowercase[pos]
         elif pos == 26:
            return ""
         elif pos == 27:
            return random.choice(string.punctuation + string.digits)

   # Random word length, according to english frequency of a space
   def genWordLength(maxLength = 100):
      spaceProba = StringGen.englishFreq[26]
      proba = [(1-spaceProba)** length * spaceProba for length in range(1, 100)]
      cums = list(StringGen.cumsum(proba))
      nb = bisect.bisect(cums, random.random()) + 1
      return min(nb, maxLength)

   # Generate a random word of a given length
   # If no length is given, a typical length will be used
   def genWord(length = -1, allowSpecials = False):
      if length == -1:
         length = StringGen.genWordLength()

      word = "".join([StringGen.genLetter(allowSpaces = False, allowSpecials = allowSpecials) for loop in range(length)])
      return word

   # Helper function : concatenate calls to genFunc() until we reach a length of "length"
   def genUpToLength(length, genFunc):
      text = ""
      while len(text) < length:
         if text != "":
            text += " "
         text += genFunc()
      text = text[:length]
      # No space at then end, never
      if text[-1] == " ":
         text = text[:-1] + StringGen.genLetter(allowSpaces = False)
      return text

   def genTitle(length = 100, allowSpecials = False):
      title = StringGen.genUpToLength(length, lambda :StringGen.genWord(allowSpecials = allowSpecials))
      title = title.title()
      return title

   def genSentence(length = 100, maxWordLength = 50, allowSpecials = False, addPonctuation = True):
      sentence = StringGen.genUpToLength(length, lambda : StringGen.genWord(length = StringGen.genWordLength(maxWordLength), allowSpecials = allowSpecials))
      # Add ponctuation at the end
      if addPonctuation:
         sentence = sentence[:length-1] + random.choice(".?!")
      sentence = sentence[:1].upper() + sentence[1:]
      return sentence


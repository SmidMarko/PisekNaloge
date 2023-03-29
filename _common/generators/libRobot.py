import os, re, sys
import copy, inspect

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

class GenAllLibs():
   allNames = []
   @staticmethod
   def generate(listFunctions):
      genDir = os.path.realpath(sys.argv[1])
      if not os.path.isdir(genDir):
         error("{} is not a directory !".format(genDir))
      directory = os.path.realpath(genDir + "/../files/lib/")
      for cls_name,cls in copy.copy(globals()).items():
         if cls_name != "GenLib" and inspect.isclass(cls) and issubclass(cls, GenLib):      
            object = globals()[cls_name]
            cl = object()
            cl.declareFunctions(listFunctions)
            
            for fileName,content in cl.export():
               fullPath = directory + "/" + fileName
               print("Writing file {}".format(fullPath))
               f = openFile(fullPath)
               f.write(content)
               f.close()



class GenLib():
   allFiles = []
   def __init__(self):
      None
   
   # List of available functions
   def declareFunctions(self, listFunctions):
      self.listFunctions = []
      for f in listFunctions:
         self.listFunctions.append(f)

   def addFile(self, fileName, content):
      self.allFiles.append((fileName, content))

   def export(self):
      self.allFiles = []
      self.genHeaderFile()
      self.genSourceFile()
      return self.allFiles

   def getHeaderFileName(self):
      return "dummy"

   def genHeaderFile(self):
      pass

   def getSourceFileName(self):
      return "dummy"

   def genSourceFile(self):
      text = self.header()
      for (name, nbArgs), translations in self.listFunctions:
         for trans in translations:
            text += self.functionDeclr(name, nbArgs, trans)
      text += self.footer()
      
      new_text = []

      for line in text.split('\n'):
         if not line.strip():
            continue
         else:
            line = re.sub(r'^      (.*)', r'\1', line)
            new_text.append(line)      
      
      text = "\n".join(new_text)

      self.addFile(self.getSourceFileName(), text)


   def header(self):
      return ""
      
   def footer(self):
      return ""
   
   def functionDeclr(self, name, nbArgs = 0):
      return ""  

class GenC(GenLib):
   def __init__(self):
      None

   def getSourceFileName(self):
      return "c/robot.h"

   def header(self):
      return\
      """
      #include <stdio.h>
      """

   def functionDeclr(self, name, nbArgs, trans):
      argsDecl = ", ".join(["int arg"+str(i) for i in range(nbArgs)]);
      argsCall = ", ".join(["arg"+str(i) for i in range(nbArgs)]);
      argsParams = "".join(["%d\\n" for i in range(nbArgs)]);
      if nbArgs > 0:
         argsCall = ", " + argsCall

      # Warnings otherwise...
      if argsParams != "":
         return\
         """
         inline void {trans}({argsDecl})
         {{
            printf("\\n");
            printf("CMD\\n");
            printf("{name}\\n");
            printf("{NB_ARGS}\\n");
            printf("{argsParams}"{argsCall});
            printf("\\n");
         }}
         """.format(trans = trans, name = name, argsDecl = argsDecl, argsParams = argsParams, argsCall = argsCall, NB_ARGS = nbArgs)
      return\
      """
      inline void {trans}({argsDecl})
      {{
         printf("\\n");
         printf("CMD\\n");
         printf("{name}\\n");
         printf("{NB_ARGS}\\n");
         printf("\\n");
      }}
      """.format(trans = trans, name = name, argsDecl = argsDecl, argsParams = argsParams, argsCall = argsCall, NB_ARGS = nbArgs)

class GenJava(GenLib):
   def __init__(self):
      None

   def getSourceFileName(self):
      return "java/Robot.java"

   def header(self):
      return\
      """
      package algorea;
      public class Robot {
      """
   def footer(self):
      return\
      """
      }
      """

   def functionDeclr(self, name, nbArgs, trans):
      argsDecl = ", ".join(["int arg"+str(i) for i in range(nbArgs)]);

      text=\
      """
         public static void {trans}({argsDecl}) 
         {{  
            System.out.println("");
            System.out.println("CMD");
            System.out.println("{name}");
            System.out.println("{nbArgs}");
      """.format(trans = trans, name = name, argsDecl = argsDecl, nbArgs = nbArgs)
      for i in range(nbArgs):
         text +=\
         """
            System.out.println({var});
         """.format(var = "arg"+str(i))
      
      text +=\
      """
            System.out.println("");
         }}
      """.format()
      return text

class GenOCaml(GenLib):
   def __init__(self):
      None

   def getSourceFileName(self):
      return "ocaml/Robot.ml"

   def getHeaderFileName(self):
      return "ocaml/Robot.mli"

   def genHeaderFile(self):
      text = ""
      for (name, nbArgs), translations in self.listFunctions:
         for trans in translations:
            if nbArgs == 0:
               sig = "unit"
            else:
               sig = " -> ".join(["int" for i in range(nbArgs)])
            text +=\
            """
            val {trans}: {sig} -> unit
            """.format(trans = trans, sig = sig)

      return self.addFile(self.getHeaderFileName(), text)


   def functionDeclr(self, name, nbArgs, trans):
      if nbArgs == 0:
         argsDecl = "()"
         sig = "unit"
      else:
         argsDecl = " ".join(["arg"+str(i) for i in range(nbArgs)]);
         sig = " -> ".join(["int" for i in range(nbArgs)])

      text =\
      """
      let {trans} {argsDecl} =
         begin
            Printf.printf "\\n";
            Printf.printf "CMD\\n";
            Printf.printf "{name}\\n";
            Printf.printf "{nbArgs}\\n";
      """.format(trans = trans, name = name, argsDecl = argsDecl, sig = sig, nbArgs = nbArgs)
      for i in range(nbArgs):
         text +=\
         """
            Printf.printf "%d\\n" {var};
         """.format(var = "arg"+str(i))

      text +=\
      """
            Printf.printf "\\n";
         end
      """

      return text


class GenPascal(GenLib):
   def __init__(self):
      None

   def getSourceFileName(self):
      return "pascal/robot.pas"
      
   def header(self):
      text = \
      """
      UNIT robot;
      INTERFACE
      """

      for (name, nbArgs), translations in self.listFunctions:
         for trans in translations:
            argsDecl = "; ".join(["arg"+str(i)+": LongInt" for i in range(nbArgs)]);
            text +=\
            """
            PROCEDURE {trans}({argsDecl});
            """.format(trans = trans, argsDecl = argsDecl)
      text +=\
      """
      IMPLEMENTATION
      """
      return text

   def functionDeclr(self, name, nbArgs, trans):
      argsDecl = "; ".join(["arg"+str(i)+": LongInt" for i in range(nbArgs)]);
      text=\
      """
      PROCEDURE {trans}({argsDecl});
      BEGIN
         WRITELN('');
         WRITELN('CMD');
         WRITELN('{name}');
         WRITELN('{nbArgs}');
      """.format(trans = trans, name = name, argsDecl = argsDecl, nbArgs = nbArgs)
      for i in range(nbArgs):
         text +=\
         """
         WRITELN({var});
         """.format(var = "arg"+str(i))
      
      text +=\
      """
         WRITELN('');
      END;
      """
      return text      

   def footer(self):
      return\
      """
      BEGIN

      END.
      """  
      
class GenPython(GenLib):
   def __init__(self):
      None

   def getSourceFileName(self):
      return "../run/robot.py"

   def functionDeclr(self, name, nbArgs, trans):
      argsDecl = ", ".join(["arg"+str(i) for i in range(nbArgs)]);
      text =\
      """
      def {trans}({argsDecl}):
         print()
         print("CMD")
         print("{name}")
         print({NB_ARGS})
      """.format(trans = trans, name = name, argsDecl = argsDecl, NB_ARGS = nbArgs)
      for i in range(nbArgs):
         text +=\
         """
         print({var})
         """.format(var = "arg"+str(i))
      text += """
         print()
      """
      return text

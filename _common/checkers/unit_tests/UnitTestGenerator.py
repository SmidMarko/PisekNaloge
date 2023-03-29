from generators import *

class UnitTestGenerator:
   id = 1
   expect = ""
   def add(self, cin, cout, ccan, expected):
      self.writeTo(cin, 'in')
      self.writeTo(cout, 'out')
      self.writeTo(ccan, 'can')
      if expected:
         expected = 100
      else:
         expected = 0
      self.expect = self.expect + str(expected) + "\n"

      self.id = self.id + 1
   def writeTo(self, content, ext):
      fullId = "{:03d}".format(self.id)
      name = "./gen-" + str(fullId) + "." + ext
      f = openFile(name)
      f.write(content.strip() + "\n")
   def finished(self):
      name = "./scores"
      f = openFile(name)
      f.write(self.expect.strip() + "\n")

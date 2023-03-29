#!/usr/bin/env python3
import logging, sys
from optparse import OptionParser
from TfmServer import *

"""
python-optcomplete

Later :
- toute les fonctions multi-dossiers (en particulier gen et test)
- generator should work if not in local directory
"""

## Configuration
TfmServer.addRepo("svn://svn.france-ioi.org/tasks/v01/")
 
## Parsing options
usage = """usage: %prog [options] cmd [name [name ...]]
Test File Managment (Server):
   Allow to easily manage tasks and tests on the evaluation server.

Examples:
  %prog analyse
  %prog update
  %prog update task_name path/to/task/dir
  %prog updateall
  %prog list

Arguments:
  cmd:     Name of the command  
     - analyse   Analyse the set of tasks and show the report
     - update    If no arguments, update the local tasks list.
                 If arguments, update the tasks with these names.
     - updateall Update all tasks, same as update X for all X in `tfms list`
     - list      List all tasks
 
  name     Full path to a task directory or name of a task"""

parser = OptionParser(usage=usage, version="%prog 1.0", epilog="Don't hesitate to propose improvments.")
parser.add_option("-l", "--log", default="INFO", choices=["DEBUG", "INFO", "WARNING", "ERROR"],
                  help="log level: DEBUG, INFO, WARNING or ERROR [default:%default]",
                  dest="logLevel", metavar="LEVEL")
parser.add_option("-d", "--dir", action="store", type="string",
                  help="tests directory that should be used",
                  dest="testsDir", metavar="DIR")
parser.add_option("-i", "--info", action="store_true",
                  help="display information about the configuration",
                  dest="displayInfo", default=False)
parser.add_option("--username", action="store", type="string",
                  help="specify a svn username ARG",
                  dest="svnUser", metavar="ARG")
parser.add_option("--password", action="store", type="string",
                  help="specify a svn password ARG",
                  dest="svnPass", metavar="ARG")
parser.add_option("--no-update-common", action="store_true",
		  dest="noUpdateCommon", help="Do not update _common")

(options, args) = parser.parse_args()

## Apply options
if len(args) < 1:
   parser.error("incorrect number of arguments")
if options.testsDir == None:
   options.testsDir = input("Local tests directory to use ? ")

TfmServer.setSvnUserPass(options.svnUser, options.svnPass)
numeric_level = getattr(logging, options.logLevel.upper(), None)
logging.getLogger().setLevel(numeric_level)

TfmServer.setLocalTestsDir(path.realpath(options.testsDir))
TfmServer.setLocalBackupDir(path.realpath(options.testsDir + "/_BACKUPS"))

#print(options)
#print(args)

## Main command
def main():
   cmdName = args[0]
   if cmdName == "update":
      if options.displayInfo:
         TfmServer.showParameters()
      if not options.noUpdateCommon:
         TfmServer.updateCommon()
      if len(args) == 1:
         TfmServer.updateLocalTasksList()
      else:
         TfmServer.updateLocalTasks(args[1:])
   elif cmdName == "analyse":
      if options.displayInfo:
         TfmServer.showParameters()
      TfmServer.analyse()
   elif cmdName == "list":
      TfmServer.list()
   elif cmdName == "updateall":
      TfmServer.updateAllLocalTasks()
   else:
      parser.print_help()

if __name__ == "__main__":
   main()

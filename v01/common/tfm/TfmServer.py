import re
import os
import shutil
import time
from getpass import getpass
import traceback

from LocalTask import *


class TfmServer:
   ### Data
   svnUSER = None
   svnPASS = None
   svnRepos = []
   svnPathCommon = {
      'root':"svn://svn.france-ioi.org/tasks/v01/",
      'path':"_common/",
      'full':"svn://svn.france-ioi.org/tasks/v01/_common/"
      }

   testsDir = None
   backupDir = None
     
   ### "public" functions
   ## Configuration
   def setSvnUserPass(username, password):
      TfmServer.svnUSER = username
      TfmServer.svnPASS = password

   def addRepo(repoPath):
      if repoPath[-1] != "/":
         repoPath = repoPath + "/"
      TfmServer.svnRepos.append(repoPath)
      TfmServer.svnRepos.sort()

   def setLocalTestsDir(localDir):
      if localDir[-1] != "/":
         localDir = localDir + "/"
      TfmServer.testsDir = localDir
      TfmCommon.setCommonDir(TfmServer.testsDir + "/_common/")

   def setLocalBackupDir(localDir):
      TfmServer.backupDir = localDir

   ## Actions
   """
   Display all the parameters
   """
   def showParameters():
      print("Parameters:")
      print("* SVN repositories:\n{}".format(textwrap.indent("\n".join(TfmServer.svnRepos), "\t")))
      print("* Local test dir:\n\t{}".format(TfmServer.testsDir))
      print("* Local backup dir:\n\t{}".format(TfmServer.backupDir))

   """
   Update the local list of all available tasks
   """
   def updateLocalTasksList():#DONE
      TfmServer.testValidity()

      allTasks = [task['path'] for task in TfmServer.getSVNTasks(False)]
      newTasks = list(set([taskDir for taskDir in allTasks if not path.exists(TfmServer.getLocalPath(taskDir))]))

      newTasks.sort()

      print("{} tasks in the SVNs and {} new ones:".format(len(allTasks), len(newTasks)))

      if len(newTasks) > 0:

         print(textwrap.indent("\n".join(newTasks), "\t"))
         print("Add all directories? [Y/n]")
         if re.search("^y(es)?$", input(), re.IGNORECASE):
            for taskDir in newTasks:
               taskDirFull = TfmServer.getLocalPath(taskDir)
               logging.info("Creating directory '%s'", taskDirFull)
               os.makedirs(taskDirFull)


   def updateCommon():
      TfmServer.updateLocalTasks(["_common"])

   """
   Update a list of local tasks.
   For each task : 
   - will process it if present only once on the SVNs
   - will compile/generate anything necessary
   """
   def updateLocalTasks(taskDirs):#DONE
      TfmServer.testValidity()
      errors = []

      for taskDir in taskDirs:
         # if task dir does not end up with '/', add it
         if taskDir[-1] != '/':
            taskDir = taskDir + '/'
         allTasks = TfmServer.getSVNTasksByPath(taskDir)
         logging.debug("tasks for %s: %s", taskDir, allTasks)
         # Extract only the basename
         # Allow for real path or only tasks' names
         taskDir = os.path.realpath(taskDir).replace(TfmServer.testsDir, '')
         taskDirFull = TfmServer.getLocalPath(taskDir)
         logging.debug("process dir: full=%s, rel=%s" % (taskDirFull, taskDir))
         if not path.exists(taskDirFull):
            logging.warning("%s is not a valid task, directory %s does not exists", taskDir, taskDirFull)
            continue
         
         matches = list(filter(lambda task:re.search("^{}/$".format(taskDir), task['path']), allTasks))
         if len(matches) > 1:
            logging.warning("Multiple matches for task '%s':\n%s", taskDir, textwrap.indent("\n".join(matches), "\t"))
         elif len(matches) == 0:
            logging.warning("The task '%s' is not in any SVN repository", taskDir)
         else:
            logging.info("Updating task '%s' from '%s'", taskDir, matches[0]['full'])
            try:
               TfmServer.exportTaskFromSvn(matches[0])
            except SystemExit:
               errors.append(taskDir)
            except:
               logging.error("Exception caught:")
               logging.error(traceback.format_exc())
               errors.append(taskDir)

      if len(errors) > 0:
         logging.warning("The following tasks could not be updated:")
         for task in errors:
            logging.warning("  %s", task)
         logging.error("Some errors occured during update")

   """
   Update all local tasks
   """
   def updateAllLocalTasks():
      TfmServer.updateLocalTasks([task['path'] for task in TfmServer.getSVNTasks(True)])

   """
   List all server tasks
   """
   def list():
      list = TfmServer.getSVNTasks(True)
      for file in list:
         print(file['path'])

   """
   Analyse local tasks and SVNS:
   - show global stats
   - find differences between local and the SVNs
   - find duplicates of task's name
   - find empty local dirs
   - find local dirs with no tests
   - find bad test files
   """
   def analyse():
      # Retrieve distant and local tasks lists
      svnTasks = [task['path'] for task in TfmServer.getSVNTasks(False)]
      localTasks = TfmServer.getLocalTasks(False)

      def line():
         print("\n"+"-"*20+"\n")
      
      ## Global infos
      print("Global stats:")
      print(" - {} tasks on the SVNs".format(len(svnTasks)))
      print(" - {} tasks on the local disk".format(len(localTasks)))

      line()

      ## Differences
      print("On the SVNs but not on disk:")
      for task in sorted(list(set(svnTasks) - set(localTasks))):
         print(" - {}".format(task))
      print("On the disk but not on the SVNs:")
      for task in sorted(list(set(localTasks) - set(svnTasks))):
         print(" - {}".format(task))

      line()
      
      ## Duplicates
      print("Duplicates (should be eliminated):")
      for task, paths in TfmServer.findDuplicatesSVNTaskDirs():
         print("* {} :\n{}".format(task, "\n".join(paths)))

      line()

      ## Empty dirs
      print("Local task task's directory with no files:")
      for task in localTasks:
         if LocalTask(TfmServer.getLocalPath(task)).isEmpty():
            print("  - {}".format(task))

      line()

      ## No-tests dirs            
      print("Non-empty local task task's directory with no tests:")
      for task in localTasks:
         LT = LocalTask(TfmServer.getLocalPath(task))
         if not LT.isEmpty() and not LT.hasTests():
            print("  - {}".format(task))

      line()

      ## Bad test files   
      print("Test files that don't respect the convention:")
      for task in localTasks:
         for f in LocalTask(TfmServer.getLocalPath(task)).listBadTests():
            print(f)


   ### "internal functions"
   """
   Test the validity of the paramaters
   """
   def testValidity():
      def internalTest():
         #if TfmServer.svnUSER == None or TfmServer.svnPASS == None:
         #   logging.error("No SVN credentials provided")
         #   return False
         if len(TfmServer.svnRepos) == 0:
            logging.error("No SVN repositories defined")
            return False
         if TfmServer.testsDir == None:
            logging.error("Local test directory is undefined")
            return False
         if TfmServer.backupDir == None:
            logging.error("Local backup directory is undefined")
            return False
         return True
      if not TfmCommon.testValidity() or not internalTest():
         exit(1)
      return True

   ## Getting paths or tasks' lists
   """
   Get the local path for a given task's name
   """
   def getLocalPath(taskName):
      return os.path.realpath(os.path.join(TfmServer.testsDir, taskName))

   """
   Get the local backup path for a given task's name
   """
   def getLocalPathBackup(taskName):
      return os.path.realpath(os.path.join(TfmServer.backupDir, taskName))

   """
   Get all the tasks present on the local disk
   Returns fullpaths or only names if "basename" is True
   """
   @memorize
   def getLocalTasks(basename = False):
      tasks = glob(TfmServer.testsDir + "/*")
      tasks = list(filter(lambda d:not re.search("^.*/_[^/]*$", d), tasks))
      return sorted(tasks)

   """
   Get all the tasks present on the SVNs
   Returns fullpaths or only names if "basename" is True
   """
   @memorize
   def getSVNTasks(fullPath = True):
      logging.info("Getting all tasks names from the SVNs")
      allDirs = []
      for repoURL in TfmServer.svnRepos:
         allDirs += TfmServer.getSVNTasksFromRepo(repoURL)
      return TfmServer.sortPathsList(allDirs, None if fullPath else repoURL)



   """
   Utility function to sort a list of paths
   """
   def sortPathsList(listPaths, root = None):
      listPaths.sort(key = lambda path:path['full'])
      return listPaths

   ## SVN stuff
   """
   Get all the tasks present on the provided svn path
   """
   @memorize
   def getSVNTasksFromRepo(svnRoot):
      logging.info("Getting tasks names from %s", svnRoot)
      allTasks = []
      def search(path, prof=0):
         subDir = TfmServer.listSVNBelowPath(path)
         for dir in subDir:
            if not dir['isTask']:
               search(dir['full'], prof+1)
            else:
               allTasks.append({
                  'root':svnRoot,
                  'path':dir['full'].replace(svnRoot, ''),
                  'full':dir['full']
               })

      search(svnRoot)

      return allTasks

   """
   Return the list of tasks/group in the given path, at depth 1
   (ie te immediate children). The path must be an absolute path.
   It returns a list of arrays, each with the following entries:
   - full: absolute path to local task
   - isTask: true if task, false if group
   """
   @memorize
   def listSVNBelowPath(path):
      logging.debug("Getting tasks names below %s", path)
      allTasks = []
      for file in sorted(TfmServer.execSVN("list", path).split("\n")):
         file = file.strip()
         if len(file) == 0:
            continue
         dirName = os.path.join(path, file)
         if file[0] in ["_", "."] or file[-1] != "/":
            continue
         isGroup = file[0].isupper() or file[:-1].isdigit()
         allTasks.append({
            'full':dirName,
            'isTask': not isGroup
         })
      return allTasks

   """
   Given a local relative, check that it corresponds to a SVN
   task and return the matching information as for getSVNTasksFromRepo:
   root, path and full. There can 0, 1 or more matches.
   """
   def getSVNTasksByPath(path):
       logging.debug("Get SVN task from %s" % path)
       def getRepos():
          allTasks = []
          for repoURL in TfmServer.svnRepos:
             allTasks.append({
                'root':repoURL,
                'path':'',
                'full':repoURL,
                'isTask': False
             })
          return allTasks

       allTasks = getRepos()
       # add the special _common
       allTasks.append({
          'root': TfmServer.svnPathCommon['root'],
          'path': TfmServer.svnPathCommon['path'],
          'full': TfmServer.svnPathCommon['full'],
          'isTask': True
       })
       matches = []
       while len(allTasks) > 0:
          # logging.debug("Refining %s", allTasks)
          newAllTasks = []
          for task in allTasks:
             if task['isTask'] and task['path'] == path:
                matches.append(task)
             elif not task['isTask'] and path.startswith(task['path']):
                subDir = TfmServer.listSVNBelowPath(task['full'])
                for dir in subDir:
                   newAllTasks.append({
                      'root': task['root'],
                      'path': dir['full'].replace(task['root'], ''),
                      'full': dir['full'],
                      'isTask': dir['isTask']
                   })
          allTasks = newAllTasks
       return matches

   """
   Export task from svn to the local directory
   It will compile/generate anything necessary
   Special treatment for the "_common" directory
   """
   def exportTaskFromSvn(svnTask):
      taskName = myBasename(svnTask['path'])
      exportDir ="/tmp/" + taskName

      # Export from the SVN to /tmp
      logging.info("Export '%s' to '%s'", svnTask['full'], exportDir)
      logging.debug("Remove '%s'", exportDir)
      shutil.rmtree(exportDir, ignore_errors = True)

      logging.debug("Retrieving data from SVN")
      TfmServer.execSVN("co", "{} {}".format(svnTask['full'], exportDir))

      restartScript = False
      if taskName == "_common":
         cmd = "cd {EXPORT_DIR} && bash updateAll.sh".format(EXPORT_DIR = exportDir)
         exitIfNot0(cmd, "Launching updateAll.sh for 'common'")

         # Copy the tfm config file
         #logging.debug("Copy current common config file to %s", exportDir + "/tfm/")
         #shutil.copy(TfmServer.getLocalPath(taskName) + "/tfm/conf.sh", exportDir + "/tfm/")

         def diffFiles(name):
            nonlocal restartScript
            logging.debug("Test modifications for the file 'tfm/%s'", name)
            # Detect modification in the template
            try:
               with open(TfmServer.getLocalPath(taskName) + "/tfm/" + name) as f1, open(exportDir + "/tfm/" + name) as f2:
                  c1 = "\n".join(f1.readlines())
                  c2 = "\n".join(f2.readlines())
                  if c1 != c2:
                     restartScript = True
                     logging.warning("The file tfm/%s has been modified.", name)
            except IOError as e:
                logging.error("While comparing file '%s'.:\n\t%s",  "/tfm/" + name, e)
                exit(1)

         # Detect modifications
         for f in glob(TfmServer.getLocalPath(taskName) + "/tfm/*.py"):
             diffFiles(path.basename(f))
      else:
         task = LocalTask(exportDir)
         task.checker.compile()
         task.generateTests()
         if task.newMode:
            filePath = "{EXPORT_DIR}/tests/files/".format(EXPORT_DIR = exportDir)
            if path.exists(filePath):
               for file in os.listdir(filePath):
                  fullName = os.path.join(filePath, file)
                  _, ext = os.path.splitext(fullName)
                  if ext == ".in" or ext == ".out":
                     os.chmod(fullName, 0o644)
         else:
            filePath = "{EXPORT_DIR}/".format(EXPORT_DIR = exportDir)
            if path.exists(filePath):
               for file in os.listdir(filePath):
                  fullName = os.path.join(filePath, file)
                  _, ext = os.path.splitext(fullName)
                  if ext == ".in" or ext == ".out":
                     os.chmod(fullName, 0o644)
         
      # Creates tests/backup directories
      # OSError because of different umask-masked mode
      try:
         os.makedirs(TfmServer.backupDir)
         os.makedirs(TfmServer.testsDir)
      except OSError:
         pass    

      # Move the task to the final destination
      taskPath = svnTask['path']
      localPath = TfmServer.getLocalPath(taskPath)
      localPathBackup = TfmServer.getLocalPathBackup(taskPath)

      if TfmServer.isModified(taskPath):
         logging.warning("Local modifications for '%s'. Update canceled.", taskPath)
         return

      logging.debug("Moving '%s' to '%s'", exportDir, localPath)
      if path.exists(localPath):
         timestamp = time.strftime("%Y-%m-%d--%H-%M-%S", time.localtime())
         logging.debug("Copying '%s' to '%s'", localPath, localPathBackup + "." + timestamp)
         shutil.copytree(localPath, localPathBackup + "." + timestamp)

      logging.debug("Remove '%s'", localPath)      
      shutil.rmtree(localPath, ignore_errors=True)
      logging.debug("Move '%s' to '%s'", exportDir, localPath)
      shutil.move(exportDir, localPath)

      if restartScript:
          logging.warning("Please restart last command, the script have changed.")
          exit(1)

   """
   Executes a given svn command with the given arguments
   """
   def execSVN(name, args):
      def getCredentials():
         if TfmServer.svnUSER != None and TfmServer.svnPASS != None:
            return [TfmServer.svnUSER, TfmServer.svnPASS]
         while True:
            svnUSER = input("SVN username: ")
            svnPASS = getpass("SVN password: ")
            cmd = "svn list --non-interactive --username '{USER}' --password '{PASS}' {ARGS}".format(USER=svnUSER, PASS=svnPASS, ARGS=args)
            status, output = executeCommand(cmd)
            if status == 0:
               TfmServer.svnUSER = svnUSER
               TfmServer.svnPASS = svnPASS
               break
         return [svnUSER, svnPASS]

      svnUser, svnPass = getCredentials()
      cmd = "svn {NAME} --username '{USER}' --password '{PASS}' {ARGS}".format(NAME=name, USER=TfmServer.svnUSER, PASS=TfmServer.svnPASS, ARGS=args)
      output = exitIfNot0(cmd, "Executing %s", "svn {} {}".format(name, args))
      return output

   def isModified(taskName):
      cmd = "svn st {}".format(TfmServer.getLocalPath(taskName))
      lines = exitIfNot0(cmd, "Executing %s", cmd)
      for line in lines.split("\n"):
         if re.search("^M ", line):
            return True
      return False
         
   ## Useful functions
   """
   Among all the given svn paths, find ones with duplicate task name.
   """
   def findDuplicatesSVNTaskDirs():
      dic = {}
      for task in TfmServer.getSVNTasks():
         name = myBasename(task['path'])
         if not name in dic:
            dic[name] = []
         dic[name].append(task['path'])
      return [(name, paths) for name, paths in dic.items() if len(paths) > 1]


   def listLocal():
      path = "/home/fioi/SVN/tasks/v01/"
      allTasks = []
      def search(path):
         for file in sorted(os.listdir(path)):
            dirName = os.path.join(path, file)
            if not os.path.isdir(dirName) or file[0] in ["_", "."]:
               continue
            if file[0].isupper() or file[0].isdigit():
               search(dirName)
            else:
               allTasks.append(dirName)
               
      search(path)
      return allTasks


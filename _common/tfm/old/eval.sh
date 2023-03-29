bg##############################################################################
##########     CONFIGURATION INFORMATIONS
##############################################################################

# Script's directory
SCRIPT_DIR="$( cd -P "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

source "${SCRIPT_DIR}/conf.sh"




# local vars : important
# todo : auto-completion
# help : on in the main, not in all commands


# todo : 
# - update : different command name
# - if no dir, current dir (cf svn for example)
# - tests solution
#
#
#
# add some verbose inside the function


##############################################################################
##########     MAIN INTERFACE
##############################################################################

tfm()#DONE
{
   if [ $# -eq 0 ] || [ "${1}" == "help" ]; then
      shift
      __tfm_help "$@"
      return 0
   fi

   local cmd="${1}"
   shift

   case "$cmd" in
      # Locally
      gen)
         __tfm_gen "$@"
         ;;
      run)
         __tfm_run "$@"
         ;;
      test)
         __tfm_test "$@"
         ;;
      # On eval server
      update)
         __tfm_update "$@"
         ;;
      analyse)
         __tfm_analyse "$@"
         ;;
      *)
         __tfm_help
         ;;
   esac 
   return $?
}

__tfm_help()
{
   case "${1}" in
      # Locally
      gen)
         echo "List of commands:
            - tfm gen tests
            - tfm gen sani
            - tfm gen check
            - tfm gen all
            - tfm gen clean
                  "
         ;;
      run)
         echo "run: run some command in the task directories
usage: tfm run <subcommand> [PATH...]

Available subcommands:
   sani   : run the sanitizer on the tests files in the directories
"
         ;;
      test)
         echo "TODO"
         ;;
      # On eval server
      update)
         echo "TODO"
         ;;
      analyse)
         echo "TODO"
         ;;
      help|*)
         echo "usage: tfm <subcommand> [args]
Test Files Management tool
Type 'tfm help <subcommand>' for help on a specific subcommand.

Available subcommands:
   gen     : generate/compile files
   run     : run the sanitizer, ...
   test    : test solutions (not implemented)
   update  : update the task list or tasks contents
   analyse : analyse SVNs and local test dir
"
         ;;
   esac
   return 0;
}

# http://www.debian-administration.org/article/317/An_introduction_to_bash_completion_part_2
# http://svn.apache.org/repos/asf/subversion/tags/0.13.2/tools/dev/bash_completion
__tfm_autocomplete() 
{
    local cur prev opts
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    

   case "${prev}" in
      help)
         opts="gen run test update analyse"
         ;;
      gen)
         opts="tests sani check all"
         ;;
      run)
         opts="sani"
         ;;
      update)
         # File name completion
         COMPREPLY=()
         return 0
         ;;
      *)
         opts="gen run test update analyse help"
         ;;
    esac

   COMPREPLY=( $(compgen -W "${opts}" -- ${cur}) )
   return 0

}
complete -F __tfm_autocomplete -o default tfm



__tfm_gen()#DONE
{
   if [ $# -eq 0 ]; then
      __tfm_help gen
      return 0
   fi

   local cmd="${1}"
   shift

   if [ "$cmd" == "tests" ]; then
      __tfm_run_on_dirs __tfm_generateTests "$@"
      return 0
   fi;
   if [ "$cmd" == "sani" ]; then
      __tfm_run_on_dirs __tfm_compileSanitizer "$@"
      return 0
   fi;
   if [ "$cmd" == "check" ]; then
      __tfm_run_on_dirs __tfm_compileChecker "$@"
      return 0
   fi;
   if [ "$cmd" == "all" ]; then
      __tfm_gen tests
      __tfm_gen sani
      __tfm_gen check
      return 0
   fi;
   if [ "$cmd" == "__tfm_clean" ]; then
      __tfm_run_on_dirs __tfm_clean "$@"
      return 0
   fi;
  
   __tfm_help gen
}


__tfm_run()#DONE
{
   if [ $# -eq 0 ]; then
      __tfm_help run
      return 0
   fi

   local cmd="${1}"
   shift

   if [ "$cmd" == "sani" ]; then
      __tfm_run_on_dirs __tfm_runSanitizer "$@"
      return 0
   fi;

   __tfm_help run
}

repeat()#Not Used 
{
    n=$1
    shift
    while [ $(( n -= 1 )) -ge 0 ]
    do
        "$@"
    done
}

__tfm_test()#Done
{
   return 0
   echo "Temporary solution...do not use !"
   solFile="${1}"

   # get all args, get the solution and the test file, compile it, run it, show results + timing and memory usage ?? (cf speed bechmarks)

   # Compilation of the solution
   g++ -O2 -W -Wall -Wno-unused-result -o sol "${solFile}"
   
   # Run tests
   for fIn in *.in
   do
      fOut=$(echo "$fIn" | sed 's/.in/.out/')
      time ./sol < $fIn > out
      #diff -bB out $fOut
      echo "DONE $fIn"
   done

   return 1
}

__tfm_compile_solution()#DONE
{
   local solFile="${1}"
   shift

   local taskDir=$(dirname ${solFile})
   local prog="${taskDir}/__prog"

   # Do the compilation of the solution
   case $(echo "${solFile}" | awk -F . '{if (NF>1) {print $NF}}') in
      c|cpp)
         if [ ! -e "${prog}" ]; then
            g++ -W -Wall -O2 -o "${prog}" "${solFile}" 
         fi
         ;;
      java)
         if [ ! -e "${prog}" ]; then
            gcj --main=Main -o "${prog}" "${solFile}" 
         fi
         ;;
      py)
         # Nothing to do
         ;;
    esac

   # For each test case, run it
   while [ $# -ne 0 ]; do
      testFile="${1}"
      if [ ! -e ${1} ]; then
         echo "'${1}' is not a valid test file";
         shift
         continue
      fi
      # TODO
   
      shift
   done;


   # Test if checker or not
}


__tfm_update()#DONE
{
   # Update _common in every case
   __tfm_update_dirs "${TESTS_DIR}/_common"

   # Update the list of directories
   if [ $# -eq 0 ]; then
      __tfm_update_list_dirs
      return 0
   fi

   # Update _common
   #if [ $# -eq 1 ] && [ $(basename "${1}") == "_common" ] ; then
   #   __tfm_exportTaskFromSvn "svn://svn.france-ioi.org/decoupage/_common"
   #   return 0
   #fi

   # Update directories
   __tfm_update_dirs "$@"
}



__tfm_update_dirs()#DONE
{
   local list=$(__tfm_getAllTasksDirsFromRepo)
   list="${list}
svn://svn.france-ioi.org/decoupage/_common/" 

   while [ $# -ne 0 ]; do
      if [ ! -e ${1} ]; then
         echo "'${1}' is not a valid test directory";
         shift
         continue
      fi
      realDir=$(cd -P ${1} && pwd)
      dirName=$(dirname "${realDir}")
      baseName=$(basename "${realDir}")
      if [ "${dirName}" != $(echo "${TESTS_DIR}" | sed 's|/$||') ]; then
         echo "'${realDir}' is not a valid test directory";
      else
         matches=$(echo "$list" | grep "/${baseName}/")
         nbMatches=$(echo "$matches" | sed "/^$/d" | wc -l)
         if [ $nbMatches -gt 1 ]; then
            echo "Multiple matches for '${baseName}':"
            echo "$matches"
         fi
         if [ $nbMatches -eq 0 ] ; then
            echo "The task '${baseName}' is not in any SVN repository";
         fi
         if [ $nbMatches -eq 1 ] ; then
            __tfm_exportTaskFromSvn "${matches}"
         fi
      fi
      shift
   done;
   return 0
}

__tfm_update_list_dirs()#DONE
{
   if [ $# -ne 0 ]; then
      __tfm_help update
      return 0
   fi
   
   local list=$(__tfm_getAllTasksDirsFromRepo)
   local listNew=""
   local nbTasks=0
   local nbTasksNew=0

   for repoDir in $list
   do
      nbTasks=$(($nbTasks + 1))
      taskDir=$(basename "${repoDir}")
      if [ ! -e "${TESTS_DIR}/${taskDir}" ]; then
         nbTasksNew=$(($nbTasksNew + 1))
         listNew="${listNew}|${taskDir}"
      fi
      #mkdir -p "${TESTS_DIR}/${taskDir}"
      #echo "$taskDir";
   done
   listNew=$(echo "$listNew" |tr '|' '\n')


   echo "${nbTasks} tasks in the SVN directories"
   echo "${nbTasksNew} new tasks"
   if [ ${nbTasksNew} -ne 0 ]; then
      echo "$listNew"
      read -p "Add all directories? " -n 1 -r
      echo ""
      if [[ $REPLY =~ ^[Yy]$ ]]
      then
         for taskDir in $listNew
         do
            echo "Creating '${TESTS_DIR}/${taskDir}'"
            mkdir -p "${TESTS_DIR}/${taskDir}"
         done
      fi
   fi;
}

__LINE()#DONE
{
   echo ""
   echo "----------------------------------------"
   echo ""
}
__tfm_analyse()#DONE
{
   local list
   local svnTasks=$(__tfm_getAllTasksDirsFromRepo | getBasename | sort)
   local localTasks=$(ls "${TESTS_DIR}" | grep -v "^_" | sort)

   ## Global infos
   if true; then
      echo "Global stats:"
      nbSvnTasks=$(echo "$svnTasks" | wc -l)
      nbLocalTasks=$(echo "$localTasks" | wc -l)
      echo "  - ${nbSvnTasks} tasks on the SVNs"
      echo "  - ${nbLocalTasks} tasks on the local disk"
      __LINE
   fi

   ## Groups
   if true; then
      echo "List of groups:"
      groups="usaco prog"
      nbNoGroup=$nbLocalTasks
      for group in $(echo "$groups" | tr ' ' '\n')
      do
         nb=$(echo "$localTasks" | grep "^${group}_" | wc -l)
         echo "  - ${group}: ${nb} tasks"
         nbNoGroup=$(($nbNoGroup - $nb))
      done
      echo "  - remaining: ${nbNoGroup} tasks"
      __LINE
   fi;

   ## Differences
   if true; then
      list=$(comm -23 <(echo "$svnTasks") <(echo "$localTasks"))
      if [ "$list" != "" ]; then
         echo "On the SVNs but not on disk:"
         for f in $list
         do
            echo "  - $f"
         done
      fi
      list=$(comm -13 <(echo "$svnTasks") <(echo "$localTasks"))
      if [ "$list" != "" ]; then
         echo "On the disk but not on the SVNs:"
         for f in $list
         do
            echo "  - $f"
         done
      fi
      __LINE
   fi

   ## Duplicates
   if true; then
      echo "Duplicates (should be eliminated):"
      local svnTasksNOB=$(__tfm_getAllTasksDirsFromRepo | sort)
      for dup in $(echo "$svnTasks" | uniq -d)
      do
         echo "* $dup :"
         echo "$svnTasksNOB" | grep "/$dup"
         echo ""
      done
      __LINE
   fi

   ## Empty dirs
   if true; then
      list=$(
         for task in $localTasks
         do
            if [ $(ls "${TESTS_DIR}/$task" | wc -l) -eq 0 ]; then
               echo "  - $task"
            fi
         done
      )
      if [ "$list" != "" ]; then
         echo "Local task task's directory with no files:"
         echo "$list"
         __LINE
      fi
   fi

   ## No-tests dirs
   if true; then
      list=$(
         for task in $localTasks
         do
            if [ ! $(ls "${TESTS_DIR}/$task" | wc -l) -eq 0 ] && [ $(find "${TESTS_DIR}/$task" -name "*.in" | wc -l) -eq 0 ]; then
               echo "  - $task"
            fi
         done
      )
      if [ "$list" != "" ]; then
         echo "Non-empty local task task's directory with no tests:"
         echo "$list"
         __LINE
      fi
   fi

   ## Bad test files
   if true; then
      list=$(
         for f in $(find "${TESTS_DIR}" -name "*.in" -o -name "*.out" | sort | grep -vE ".*/[^\./]{1,}\.[0-9]{2,}\.(in|out)")
         do
            echo "  - $f"
         done
      )
      if [ "$list" != "" ]; then
         echo "Test files that don't respect the convention:"
         echo "$list"
         __LINE
      fi
   fi

   return 1
}


# todo, check dir exists and is a valid test one dir
# Run a function on all directories in argument
__tfm_run_on_dirs()
{
   if [ $# -eq 0 ]; then
      echo "Internal error, __tfm_run"
      exit 1
   fi

   local fct="${1}"
   shift

   while [ $# -ne 0 ]; do
      echo "${1} :"
      $fct ${1}
      shift
   done;
   return 0
}


##############################################################################
##########     ACTION FUNCTIONS
##############################################################################





# Generate the tests of a given task
# Example :
# __tfm_generateTests taskDir
__tfm_generateTests()#DONE
{
   ## Checks
   checkNbArgs $FUNCNAME $# 1 || return 1
   _error_dir_not_exists "${1}" || return 1

   ## Local variables
   local taskDir="${1}"
   local genPython="generator.py"
   local genScript="gen.sh"
   local res=0
   
   ## Actions
   pushd "${taskDir}" > /dev/null

   # Python generator
   if [ -e "${genPython}" ]
   then
      export PYTHONPATH="${COMMON_DIR}/generators/"
      python3 -B "${genPython}" .
   # gen.sh script
   elif [ -e "${genScript}" ]
   then
      bash ${genScript}
   fi

   popd > /dev/null

   return "$res"
}


# Compile the checker of a given task
# Example :
# __tfm_compileChecker taskDir
__tfm_compileChecker()#DONE
{
   ## Checks
   checkNbArgs $FUNCNAME $# 1 || return 1
   _error_dir_not_exists "${1}" || return 1

   ## Local variables
   local taskDir="${1}"
   local checker="check.cpp"
   local res=0

   ## Actions
   pushd ${taskDir} > /dev/null

   if [ -e "${checker}" ]
   then
      g++ -O2 -W -Wall -Wno-unused-result -I${COMMON_DIR}/checkers -o check ${checker}
      if [ $? -ne 0 ]; then
         res=1
      fi
   fi
   
   popd > /dev/null

   return "$res"
}


# Compile the sanitizer  of a given task
# Example :
# __tfm_compileSanitizer taskDir
__tfm_compileSanitizer()#DONE
{
   ## Checks
   checkNbArgs $FUNCNAME $# 1 || return 1
   _error_dir_not_exists "${1}" || return 1

   ## Local variables
   local taskDir="${1}"
   local sanitizer="sanitizer.cpp"
   local res=0

   ## Actions
   pushd ${taskDir} > /dev/null

   if [ -e "${taskDir}/${sanitizer}" ]
   then
      g++ -std=c++0x -O2 -W -Wall -Wno-unused-result -I${COMMON_DIR}/sanitizer -o sanitizer ${sanitizer}
      if [ $? -ne 0 ]; then
         res=1
      fi
   fi

   popd > /dev/null

   return "$res"
}


# Run the sanitizer on all inputs of a given task
# Example :
# __tfm_runSanitizer taskDir
__tfm_runSanitizer()#DONE
{
   ## Checks
   checkNbArgs $FUNCNAME $# 1 || return 1
   _error_dir_not_exists "${1}" || return 1

   ## Local variables
   local taskDir="${1}"
   local sanitizer="sanitizer.cpp"
   local res=0

   ## Actions
   pushd ${taskDir} > /dev/null

   if [ -e "${sanitizer}" ]
   then
      __tfm_compileSanitizer "${taskDir}"

      # Test all files
      for inFile in *.in
      do
         echo "CHECKING ${taskDir}/${inFile}"
         ./sanitizer < "$inFile"
         if [ $? -ne 0 ]; then
            echo "INVALID INPUT"
            res=1
            break
         fi
      done

      rm -f sanitize
   fi

   popd > /dev/null

   return "$res"
}


# Remove all generated files of a given task
# Example :
# __tfm_clean taskDir
__tfm_clean()#DONE
{
   ## Checks
   checkNbArgs $FUNCNAME $# 1 || return 1
   _error_dir_not_exists "${1}" || return 1

   ## Local variables
   local taskDir="${1}"
   local res=0

   ## Actions
   pushd ${taskDir} > /dev/null

   # Generated test files
   rm -f gen-*.{in,out}

   # All executables files at the root
   find . -maxdepth 1 -type f -perm +111 -delete

   popd > /dev/null

   return "$res"
}

##############################################################################
##########     SVN FUNCTIONS
##############################################################################

# Get all existing tasks' TextId of a specific SVN directory
# Possible to explore the directory with specific depth
# Example :
# __tfm_getTasksDirsFromRepo "svn://svn.france-ioi.org/decoupage" 0
__tfm_getTasksDirsFromRepo()#DONE
{
   local svnPath="${1}"
   local depth="${2}"
   
   if [ $depth -eq -1 ]; then
      return 0
   fi;

   local list=$(svn list "${svnPath}" | grep -e "/$" | grep -ve "^_")
   for x in $list 
   do
      echo "$svnPath/$x";
   done;
   # Later recursion with depth !
}

# Get all existing tasks' TextId of all referenced SVN directories
# Example :
# __tfm_getAllTasksDirsFromRepo
# Example with forcing refresh :
# __tfm_getAllTasksDirsFromRepo 1
__tfm_getAllTasksDirsFromRepo()#DONE
{
   __tfm_getTasksDirsFromRepo "svn://svn.france-ioi.org/decoupage" 0
   __tfm_getTasksDirsFromRepo "svn://svn.france-ioi.org/lycees" 0
   __tfm_getTasksDirsFromRepo "svn://svn.france-ioi.org/lycees/_Archived" 0
   return 0
}











## to improve
__tfm_exportTaskFromSvn()#DONE
{
   taskPath="${1}"
   taskDir=$(basename $taskPath)
   exportDir="/tmp/${taskDir}"

   #" Getting task from svn
   echo "Exporting ${taskPath}"
   rm -rf "${exportDir}"
   svn export  --username "${SVN_USER}" --password "${SVN_PASS}" "${taskPath}" "${exportDir}" > /dev/null
   if [ $? -ne 0 ]; then
      echo "ERROR"
      return 1
   fi

   ## Doing stuff on task
   case "${taskDir}" in
      _common)
         pushd "${exportDir}"
         bash updateAll.sh

         # Copy the tfm config file
         cp "${TESTS_DIR}/_common/tfm/conf.sh" tfm/
         # Detect modification in the template
         diff "${TESTS_DIR}/_common/tfm/conf.template.sh" "tfm/conf.template.sh" > /dev/null
         if [ $? -ne 0 ]; then
            echo "WARNING: the tfm/conf.template.sh has been modified !"
         fi
         # Detec modification in the script itself
         diff "${TESTS_DIR}/_common/tfm/eval.sh" "tfm/eval.sh" > /dev/null
         if [ $? -ne 0 ]; then
            echo "WARNING: the tfm/eval.sh has been modified !"
            echo "         Please include the tfm once again"
         fi

         popd
         ;;
      *)
         echo "   - compiling checker"
         __tfm_compileChecker "${exportDir}"
         if [ $? -ne 0 ]; then
            echo "ERROR"
            return 1
         fi

         echo "   - generating tests"
         __tfm_generateTests "${exportDir}"
         if [ $? -ne 0 ]; then
            echo "ERROR"
            return 1
         fi

         echo "   - chmod"
         chmod 644 ${exportDir}/*.in ${exportDir}/*.out
         ;;
   esac 

   #" Export the task to tests directory
   mkdir -p "${BACKUP_DIR}"
   mkdir -p "${TESTS_DIR}"

   if [ -e "${TESTS_DIR}/${taskDir}" ]; then
      timestamp=$(date +"%Y-%m-%d--%H-%M-%S")
      mv "${TESTS_DIR}/${taskDir}" "${BACKUP_DIR}/${taskDir}.${timestamp}"
   fi

   rm -rf "${TESTS_DIR}/${taskDir}"
   mv "${exportDir}" "${TESTS_DIR}/${taskDir}"
}



##############################################################################
##########     UTILITY FUNCTIONS
##############################################################################

# Used to check the number of arguments of a function
# Example :
# checkNbArgs $FUNCNAME $# 2
checkNbArgs()#DONE
{
   if [ $# -ne 3 ]; then
       echo "'checkNbArgs' requires 3 arguments !"
       return 1;
   fi
   if [ $2 -ne $3 ]; then
       echo "$3 arguments needed for function '$1'"
       return 1;
   fi
}

_error_dir_not_exists()#DONE
{
   checkNbArgs $FUNCNAME $# 1 || return 1
   if [ ! -e "${1}" ]
   then
      echo "The directory '${1}' does not exists !"
      return 1
   fi
   return 0
}

getBasename()#DONE
{
   while read var;
   do
      basename "$var"
   done
}






##############################################################################
##########     Not used anymore
##############################################################################
__filterGrep()#NOT_NEEDED
{
   if [ $# -eq 0 ]; then
       echo "At least one regex needed for 'filterGrep'"
       return 1;
   fi

   all=$(__tfm_getAllTasksDirsFromRepo)

   while [ $# -ne 0 ]; do
      for task in $all
      do
         base=$(basename $task)
         echo $base | grep -E "${1}" > /dev/null 2>&1
         if [ $? -eq 0 ]; then
            echo $task
         fi
      done;
      shift
   done
}
__listPossibles()#NOT_NEEDED
{
   if [ $# -eq 0 ]; then
       echo "At least one regex needed for 'listPossibles'"
       return 1;
   fi

   all=$(filterGrep "$@")
   nb=$(echo $all | tr ' ' '\n' | wc -l)

   if [ $nb -gt 20 ]; then
      read -p "${nb} possibilities. List all? " -n 1 -r
      echo ""
      if [[ $REPLY =~ ^[Nn]$ ]]
      then
         return 0
      fi
   fi

   for path in $all
   do
      echo "${path}"
   done
}
__retrieveAll()#NOT_NEEDED
{
   local regex="${1}"

   for path in $(filterGrep ".*de(s|_).*")
   do
      __tfm_exportTaskFromSvn "${path}"
   done
}






# Ensure we exit correctly if an error occurs
#set -o errexit

# Config
export PYTHONPATH="../:../../../generators/"
alias python3="python3 -B"

for d in *
do
   if [ -d "$d" ]; then
      echo "Working on $d"
      cd "$d";
      echo "   - compiling checker"
      g++ -W -Wall -o check check.cpp
      echo "   - generating tests"
      python3 -B generator.py .;
      echo "   - checking"
      rm -f tmp_scores
      rm -f full_output
      for f in *.in
      do
         base=$(echo "$f"| sed 's/.in//')
         ./check "$base.can" "$base.in" "$base.out" > tmp
         head -n 1 tmp >> tmp_scores
         cat tmp >> full_output
      done
      rm -f tmp
      diff tmp_scores scores > _diff
      if [ $? -ne 0 ]; then
         echo ""
         cat _diff
         echo "ERROR !"
         exit 0
      fi
      echo "   - cleaning"
      rm _diff
      rm -f *.in *.out *.can tmp_scores full_output scores check
      cd ..;
   fi
done


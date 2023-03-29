#! /usr/bin/env 

#
# Test Cases
#

SuiteTest() {
   shuRegTest Init

   shuRegTest TestOutputProvided

   # With solution in all supported languages
   shuRegTest TestOutputWithC
   shuRegTest TestOutputWithCpp
   shuRegTest TestOutputWithJava
   shuRegTest TestOutputWithJvs
   shuRegTest TestOutputWithPython

   shuRegTest TestRandomTests
   shuRegTest TestOneTest
   shuRegTest TestExternalGenerator

   shuRegTest Clean
}

SuiteGenerate() {
   shuRegTest Init

   shuRegTest TestGenerateOutputProvided

   # With solution in all supported languages
   shuRegTest TestGenerateOutputWithC
   shuRegTest TestGenerateOutputWithCpp
   shuRegTest TestGenerateOutputWithJava
   shuRegTest TestGenerateOutputWithJvs
   shuRegTest TestGenerateOutputWithPython

   shuRegTest TestGenerateRandomTests
   shuRegTest TestGenerateOneTest
   shuRegTest TestGenerateExternalGenerator

   shuRegTest Clean
}

Init()
{
   export PYTHONPATH=../
   Clean
}

Clean()
{
   shuFailedTestCount=`expr ${shuTestNbr} - ${SHU_TOTAL_NR_SUCCEEDED}`
   if [ $shuFailedTestCount -eq 1 ]
   then
      rm -f errLog
   fi
   rm -f Main.java
}

TestOutputProvided()    { RunGeneratorTest "OutputProvided";   }
TestOutputWithC()       { RunGeneratorTest "OutputWithC";      }
TestOutputWithCpp()     { RunGeneratorTest "OutputWithCpp";    }
TestOutputWithJava()    { RunGeneratorTest "OutputWithJava";   }
TestOutputWithJvs()     { RunGeneratorTest "OutputWithJvs";    }
TestOutputWithPython()  { RunGeneratorTest "OutputWithPython"; }
TestRandomTests()       { RunGeneratorTest "RandomTests";      }
TestOneTest()           { RunGeneratorTest "OneTest";          }
TestExternalGenerator() { RunGeneratorTest "ExternalGenerator";}

TestGenerateOutputProvided()    { RunGenerator "OutputProvided";     }
TestGenerateOutputWithC()       { RunGenerator "OutputWithC";        }
TestGenerateOutputWithCpp()     { RunGenerator "OutputWithCpp";      }
TestGenerateOutputWithJava()    { RunGenerator "OutputWithJava";     }
TestGenerateOutputWithJvs()     { RunGenerator "OutputWithJvs";      }
TestGenerateOutputWithPython()  { RunGenerator "OutputWithPython";   }
TestGenerateRandomTests()       { RunGenerator "RandomTests";        }
TestGenerateOneTest()           { RunGenerator "OneTest";           }
TestGenerateExternalGenerator() { RunGenerator "ExternalGenerator"; }


#
# Needed functions
#

RunGenerator()
{
   NAME="${1}"

   mkdir -p refs

   python3 "Gen${NAME}.py" tests > "refs/${NAME}.stdout"
   shuAssert "Expected correct generator." $?
   cat tests/* > "refs/${NAME}.inout"
   rm -rf tests
}

RunGeneratorTest()
{
   NAME="${1}"

   python3 "Gen${NAME}.py" tests > "${NAME}.stdout"
   shuAssert "Expected correct generator." $?

   cat tests/* > "${NAME}.inout"
   rm -rf tests
   
   diff "refs/${NAME}.inout"   "${NAME}.inout" >> errLog
   shuAssert "Expected identical in/out files." $?
   diff "refs/${NAME}.stdout"  "${NAME}.stdout" >> errLog
   shuAssert "Expected identical generator output." $?
   
   rm -f "${NAME}.inout" "${NAME}.stdout"
}

#
# Main
#

. "../ext/shUnit/shUnit"

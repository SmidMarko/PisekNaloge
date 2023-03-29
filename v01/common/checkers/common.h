#ifndef COMMON_H
#define COMMON_H

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cstdarg>
#include <cctype>

#include <string>
#include <vector>
#include <limits>

#include <iostream>
#include <iomanip>

using namespace std;

/*
Generic checker class.
*/
class Checker {
public:
   // Candidat output, reference input, reference output
   FILE * fOut, * fRefIn, * fRefOut;

   // Constructor & destructor
   Checker(int argc, char ** argv);
   virtual ~Checker();

   // Set the debug mode
   void setDebug(bool isDebug);

   // Main function to call
   void run();

   // Utility function to format texte printf-like
   string format(string fmt, ...);

protected:   
   bool _isDebug;

   // The function that will need to be overloaded in derived classes
   virtual void check() = 0;

   // Are we in debug mode ?
   bool isDebug();
   
   // The answer is correct
   virtual void success(string msg = "", int score = 100);

   // The answer is invalid
   virtual void failure(string msg = "", int score = 0);

   // Error that can't be the user fault : bad lib, bad input...
   void internalError(string msg);

   // Display the score and then the message
   virtual void displayScore(int score, string msg = "");

   // If a message is provided it will write it
   // Else if will look at the stack "OutputMessages" and
   // write each of them
   virtual void displayMessage(string msg = "");

   void addMessage(string msg);
   // Checker messages
   vector<string> OutputMessages;
};


Checker::Checker(int argc, char ** argv) {
   if (argc != 4) {
      printf("3 arguments needed :\n");
      printf("  - candidate output\n");
      printf("  - reference input\n");
      printf("  - reference output\n");          
      exit(1);
   }
   fOut     = fopen(argv[1],"r");
   fRefIn   = fopen(argv[2],"r");
   fRefOut  = fopen(argv[3],"r");
   if (fOut == NULL || fRefIn == NULL || fRefOut == NULL) {
      failure("Internal error while opening files...");
   }
   _isDebug = false;
}

Checker::~Checker() {
   if (fOut != NULL) {
      fclose (fOut);
   }
   if (fRefIn != NULL) {
      fclose (fRefIn);
   }
   if (fRefOut != NULL) {
      fclose (fRefOut);
   }
}


void Checker::setDebug(bool isDebug) {
   _isDebug = isDebug;
}

bool Checker::isDebug() {
   return _isDebug;
}

void Checker::success(string msg, int score) {
   displayScore(score, msg);
   throw 0;
}

void Checker::failure(string msg, int score) {
   displayScore(score, msg);
   throw 1;
}

void Checker::internalError(string msg) {
   failure(format("Internal error of the checker: %s\n", msg.c_str()));
}

void Checker::run() {
   try {
      check();
   }
   catch (int) {
      // Catch any exception thrown by success/failure...
      // We can exit the program
   }
}

string Checker::format(string fmt, ...) {
   char buf[2000];
   va_list ap;
   va_start(ap, fmt); /* Initialize the va_list */
   vsprintf(buf, fmt.c_str(), ap); /* Call vprintf */
   va_end(ap); /* Cleanup the va_list */
   return string(buf);
}

void Checker::displayScore(int score, string msg) {
   printf("%d\n", score);
   displayMessage(msg);
}

void Checker::displayMessage(string msg) {
   if(msg.size() > 0) {
      printf("%s\n", msg.c_str());
   }
   else {
      for(int i = 0 ; i < (int)OutputMessages.size(); i++) {
         printf("%s\n", OutputMessages[i].c_str());
      }
   }
}

void Checker::addMessage(string msg) {
   OutputMessages.push_back(msg);
}



/*
Utility class
*/
class DistChecker : public Checker 
{
protected:
   // Number of values read
   int valueIndex;   
   
   // Should we indicates how many values where outputted by the user ?
   bool _outputNbValues;
   
   // Constructor
   DistChecker(int argc, char ** argv):Checker(argc, argv)
   {
      valueIndex = 0;
      _outputNbValues = true;
   }
   
   // Setter
   void setOutputNbValues(bool value)
   {
      _outputNbValues = value;
   }   
   
   // Read one double value from the candidate's output and the reference's output
   bool readOneDouble(double &result, double &expected)
   {
      char sep;
      const int refSuccess = fscanf(fRefOut, "%lf", &expected);
      const int canSuccess = fscanf(fOut, "%lf%c", &result, &sep);

      if (refSuccess == 0)
      {
         string msg = format("Il n'y a que %d valeur(s) dans le fichier de reference. Il en manque.", valueIndex);
         OutputMessages.push_back(msg);
         return false;
      } 
      if (canSuccess == 0)
      {
         string msg = format("Il n'y a que %d valeur(s) dans votre reponse, il en manque. Ou alors votre valeur numéro %d n'est pas un décimal.", valueIndex, valueIndex+1);
         OutputMessages.push_back(msg);
         return false;
      }
      if (isnan(result)) {
         string msg = format("Le nombre à la position %d dans votre sortie n'est pas un décimal valide.", valueIndex + 1);
         OutputMessages.push_back(msg);
         return false;
      }
      
      // There was a caracter that was not a space
      if (!isspace(sep) && canSuccess != 1)
      {
        string msg = format("Dans votre réponse, les valeurs %d et %d sont collées.", valueIndex, valueIndex + 1);
         OutputMessages.push_back(msg);
         return false;
      }     
      valueIndex = valueIndex + 1;
     
      return true;
   }

   bool readOneInt(int &result, int &expected)
   {
      double resultD, expectedD;
      char sep;
      const int refSuccess = fscanf(fRefOut, "%lf", &expectedD);
      const int canSuccess = fscanf(fOut, "%lf%c", &resultD, &sep);
    
      if (abs(resultD - (int)resultD) > 1e-8)
      {
         string msg = format("La valeur %d n'est pas un entier !.", valueIndex);
         OutputMessages.push_back(msg);
         return false;
      }
      if (refSuccess == 0)
      {
         string msg = format("Il n'y a que %d valeur(s) dans le fichier de reference. Il en manque.", valueIndex);
         OutputMessages.push_back(msg);
         return false;
      } 
      if (canSuccess == 0)
      {
         string msg = format("Il n'y a que %d valeur(s) dans votre reponse, il en manque. Ou alors votre valeur numéro %d n'est pas un entier.", valueIndex, valueIndex+1);
         OutputMessages.push_back(msg);
         return false;
      }
      if (!isspace(sep) && canSuccess != 1)
      {
        string msg = format("Dans votre réponse, les valeurs %d et %d sont collées.", valueIndex, valueIndex + 1);
         OutputMessages.push_back(msg);
         return false;
      }  
      result = (int)resultD;
      expected = (int)expectedD;
     
      valueIndex = valueIndex + 1;
     
      return true;
   }   
   
   bool analyseDist(double dist, double limit, double result, double expected)
   {
      if (dist > limit)
      {
         const double dist = abs(result - expected);
         const int distLog = (int)ceil(-log(dist)/log(10));
         if (_outputNbValues)
         {
            string msg = format("Nombre total de valeurs affichées : %d", countRemainingDoubles(valueIndex));
            OutputMessages.push_back(msg);
         }
         if(distLog > 0)
         {
            string msg = format("Votre valeur numero %%d (%%.10f) est à une distance supérieure à %%.%df de la reponse attendue.",
               distLog);            
            msg = format(msg.c_str(), valueIndex, result, pow(10, -distLog), expected);
            OutputMessages.push_back(msg);
         }
         else
         {
            string msg = format("Votre valeur numero %d (%.10f) est à une distance supérieure à %.1f de la reponse attendue.",
               valueIndex, result, ceil(pow(10, -distLog)), expected);
            OutputMessages.push_back(msg);
         }
         return false;
      }
      return true;
   }   
   
   /*
   Count the number of values that can still be read from the candidat output file
   */
   int countRemainingDoubles(int nbValues = 0)
   {
      double result;
      while (fscanf(fOut, "%lf", &result) == 1)
         nbValues++;
      return nbValues;  
   }

   int powerOf10(double x)
   {
      return (int)ceil(log(x)/log(10));
   }
};


/*
Will check the validity of 'nbValues' double with a precision of 'precision'
*/
class DistDoubleChecker : public DistChecker
{
public:
   // Constructor
   DistDoubleChecker(int argc, char ** argv):DistChecker(argc, argv)   {}
   
   // Setter   
   void setUseSignificantFigures(bool value)
   {
      _useSignificantFigures = value;
   }   
   
   // Main check function
   void run(int _nbValues, double _precision = 1e-8)
   {
      precision = _precision;
      nbValues = _nbValues;
      DistChecker::run();
   }

protected:   
   double precision;
   int nbValues;
   
   void check()
   {
      bool good = true;
      for(int i = 0 ; good && i < nbValues ; i++)
         good &= checkDistDouble(precision);
      if(good)
         success();
      else
         failure();
   }
   

protected:   
   // Should we use significant figures instead of "basic precision" ?
   bool _useSignificantFigures;
   

   
   bool checkDistDouble(double precision = 1e-08)
   {
      double result, expected;
      if (!readOneDouble(result, expected))
         return false;

      double dist;
      if(_useSignificantFigures)
      {
         const double p10 = pow(10, powerOf10(expected));
         dist = abs(result / p10 - expected / p10);
      }
      else
      {
         dist = abs(result - expected);  
      }      
      return analyseDist(dist, precision, result, expected);
   }
   
  
   
};


class DistIntChecker : public DistChecker
{
public:
   // Constructor
   DistIntChecker(int argc, char ** argv):DistChecker(argc, argv)   {}
   
   // Main check function
   void run(int _nbValues)
   {
      nbValues = _nbValues;
      DistChecker::run();
   }

protected:

   int nbValues;
   
   void check()
   {
      bool good = true;
      for(int i = 0 ; good && i < nbValues ; i++)
         good &= checkDistInt();
      if(good)
         success();
      else
         failure();
   }
   

protected:   
   bool checkDistInt()
   {
      int result, expected;
      if (!readOneInt(result, expected))
         return false;

      double dist = abs(result - expected);
      return analyseDist(dist, 0.1, result, expected);
   }

   bool analyseDist(double dist, double limit, double result, double expected)
   {
      if (dist > limit)
      {
         const double dist = abs(result - expected);
         const int distLog = (int)ceil(-log(dist)/log(10));
         if (_outputNbValues)
         {
            string msg = format("Nombre total de valeurs affichées : %d", countRemainingDoubles(valueIndex));
            OutputMessages.push_back(msg);
         }
         if(distLog > 0)
         {
            string msg = format("INTERNAL ERROR: contact us");
            OutputMessages.push_back(msg);
         }
         else
         {
            string msg = format("Votre valeur numero %d (%d) est à une distance supérieure à %d de la reponse attendue.", valueIndex, (int)result, (int)ceil(pow(10, -distLog)), expected);
            OutputMessages.push_back(msg);
         }
         return false;
      }
      return true;
   }
};


class DistAllRoundChecker : public DistChecker
{
public:
   // Constructor
   DistAllRoundChecker(int argc, char ** argv):DistChecker(argc, argv),multipleThreshold(1e-5)
   {
      _cachedPower = -1;
   }
   
   void run(int _nbValues, int _nbDigits = 0)
   {
      nbValues = _nbValues;
      nbDigits = _nbDigits;
      DistChecker::run();
   }
   

protected:

   int nbValues, nbDigits;
   
   // Main check function
   void check()
   {
      bool good = true;
      for(int i = 0 ; good && i < nbValues ; i++)
         good &= checkFloor(nbDigits);
      if(good)
         success();
      else
         failure();
   }   

protected:   
   const double multipleThreshold;
   
   int _cachedPower;
   
   // Get the power factor, corresponding to the number of digits
   int getPower(int nbDigits = 0)
   {
      if (_cachedPower == -1)
         _cachedPower = (int)pow(10, nbDigits);
      return _cachedPower;
   }
   
   // Floor "value" at nbDigits decimal digits
   double getFloored(double value, int nbDigits = 0)
   {
      const int power = getPower(nbDigits);
      return floor(value * power) / power;
   }
   
   // Ceil "value" at nbDigits decimal digits   
   double getCeiled(double value, int nbDigits = 0)
   {
      const int power = getPower(nbDigits);
      return ceil(value * power) / power;
   }
   
   // Round "value" at nbDigits decimal digits   
   double getRounded(double value, int nbDigits = 0)
   {
      const int power = getPower(nbDigits);
      return round(value * power) / power;
   }   
   
   virtual vector<double> getPossibleValues(double expected, double nbDigits) = 0;

   
   bool checkFloor(int nbDigits = 0)
   {
      if (nbDigits < 0)
         failure("'nbDigits' must be a positive number !");
      
      double result, expected;
      if (!readOneDouble(result, expected))
         return false;

      if(isDebug())
         cout << expected << " " << result << endl;

      // Look for possible values
      vector<double> ValidValues = getPossibleValues(expected, nbDigits);
      
      // Look for the closest one
      double bestDist = numeric_limits<double>::infinity();
      for (int i = 0; i < (int)ValidValues.size(); i++)
      {
         if(isDebug())
            cout << "POSSIBLE :" << ValidValues[i] << endl;
         const double tmpDist = abs(result - ValidValues[i]);
         if (tmpDist < bestDist)
         {
            bestDist = tmpDist;
            expected = ValidValues[i];            
         }
      }
      if(isDebug())
         cout << "Closest :" << expected << " at " << bestDist << endl;

      return analyseDist(bestDist, 1e-8, result, expected);
   } 
   
};



class DistFloorChecker : public DistAllRoundChecker
{
public:
   DistFloorChecker(int argc, char ** argv):DistAllRoundChecker(argc, argv){}
   
   vector<double> getPossibleValues(double expected, double nbDigits)
   {
      const double threshold = multipleThreshold / getPower(nbDigits);
      
      const double floored = getFloored(expected, nbDigits);
      const double ceiled  = getCeiled(expected, nbDigits);
      
      vector<double> ValidValues(1, floored);
     
      if ( expected - floored < threshold)
         ValidValues.push_back(floored - 1.0 / getPower(nbDigits) );
      if ( ceiled - expected  < threshold && (ceiled - floored) > 0.5 / getPower(nbDigits))
         ValidValues.push_back(floored + 1.0 / getPower(nbDigits) );
      return ValidValues;
   }
   
};

class DistCeilChecker : public DistAllRoundChecker
{
public:
   DistCeilChecker(int argc, char ** argv):DistAllRoundChecker(argc, argv){}
   
   vector<double> getPossibleValues(double expected, double nbDigits)
   {
      const double threshold = multipleThreshold / getPower(nbDigits);
      
      const double floored = getFloored(expected, nbDigits);
      const double ceiled  = getCeiled(expected, nbDigits);
      
      vector<double> ValidValues(1, ceiled);
      
      if(isDebug())
      {
         cout << "threshold : " << threshold << endl;
         cout << "ceiled : " << ceiled << endl;
         cout << "floored : " << floored << endl;
         cout << "ceiled - expected : " << ceiled - expected << endl;
         cout << "expected - floored : " << expected - floored << endl;
         
      }

      if ( ceiled - expected < threshold)
         ValidValues.push_back(ceiled + 1.0 / getPower(nbDigits) );
      if ( expected - floored < threshold && (ceiled - floored) > 0.5 / getPower(nbDigits))
         ValidValues.push_back(ceiled - 1.0 / getPower(nbDigits) );
      return ValidValues;
   }
};


class DistRoundChecker : public DistAllRoundChecker
{
public:
   DistRoundChecker(int argc, char ** argv):DistAllRoundChecker(argc, argv){}
   
   vector<double> getPossibleValues(double expected, double nbDigits)
   {
      const double threshold = multipleThreshold / getPower(nbDigits);
      
      const double rounded = getRounded(expected, nbDigits);
      const double floored = getFloored(expected, nbDigits);
      const double ceiled  = getCeiled(expected, nbDigits);
      
      vector<double> ValidValues;
      
      
      if(isDebug())
      {
         cout << "abs(expected - rounded) : " << abs(expected - rounded) << endl;
         cout << "0.5 / getPower(nbDigits) : " << 0.5 / getPower(nbDigits) << endl;
         cout << "abs(0.5 / getPower(nbDigits) - abs(expected - rounded)) : " << abs(0.5 / getPower(nbDigits) - abs(expected - rounded)) << endl;
         cout << "threshold : " << threshold << endl;
      }

      // very close from the "0.5" value
      if ( abs(0.5 / getPower(nbDigits) - abs(expected - rounded)) < threshold)
      {
         ValidValues.push_back(floored);
         ValidValues.push_back(ceiled);
      }
      else
      {
         ValidValues.push_back(rounded);
      }

      return ValidValues;
   }
};
#include "checker_commands.h"
#include "checker_robot.h"




#endif

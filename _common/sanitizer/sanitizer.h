#ifndef SANITIZER_H
#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <limits>
#include <regex>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>
#include "BuffChecker.h"


// Creates the necessary buffer
FastIO BuffCheck;
// passer ça en static, plus besoin de le créer

// Useful shortcuts
#define _STB(B) ((B)?"True":"False")
#define _STBN(B) ((B)?"not ":"")


//// Utilities ////
// Build a vector from a variable number of arguments
template<typename T>
void buildVectorRec(std::vector<T> & res, T value)
{
   res.push_back(value);
}
template<typename T, typename... Args>
void buildVectorRec(std::vector<T> & res, T value, Args... args)
{
   res.push_back(value);
   buildVectorRec(res, args...);
}
template<typename T, typename... Args>
std::vector<T> buildVector(Args... args)
{
   std::vector<T> res;
   buildVectorRec(res, args...);
   return res;
}



//// List of all the checkers ////
template<typename T, typename C> 
inline void check_greater(const C & caller, const T val, const T minVal, bool _true = true)
{
   const bool good = (val >= minVal); 
   if (!good == _true)
   {
      std::stringstream ss;
      ss << "The condition " << val << " >= " << minVal << " should be " << _STB(_true);
      caller.error(ss.str());
   }
}

template<typename T, typename C> 
inline void check_smaller(const C & caller, const T val, const T maxVal, bool _true = true)
{
   const bool good = (val <= maxVal); 
   if (!good == _true)
   {
      std::stringstream ss;
      ss << "The condition " << val << " <= " << maxVal << " should be " << _STB(_true);
      caller.error(ss.str());
   }
}

template<typename T, typename C> 
inline void check_in(const C & caller, const T val, const T minVal, const T maxVal, bool _true = true)
{
   const bool good = (val >= minVal) && (val <= maxVal); 
   if (!good == _true)
   {
      std::stringstream ss;
      ss << "The condition " << minVal << " <= " << val << " <= " << maxVal << " should be " << _STB(_true);
      caller.error(ss.str());
   }
}

template<typename T, typename C, typename U> 
inline void check_in(const C & caller, const T val, const U & values, bool _true = true)
{
   const bool good = (std::find(values.begin(), values.end(), val) != values.end()); 
   if (!good == _true)
   {
      std::stringstream ss;
      ss << "The value " << val << " is " << _STBN(_true) << "present in (";
      for (unsigned int i = 0 ; i < values.size(); i++)
      {
         if (i != 0)
            ss << ",";
         ss << values[i];
      }
      ss << ")";
      caller.error(ss.str());
   }
}  

template<typename C> 
inline void check_in(const C & caller, const char val, const char* values, bool _true = true)
{
   check_in(caller,val, std::string(values), _true);
}

template<typename T, typename C> 
inline void check_equal(const C & caller, const T val, const T valEq, bool _true = true)
{
   const bool good = (val == valEq); 
   if (!good == _true)
   {
      std::stringstream ss;
      ss << "The condition " << val << " == " << valEq << " should be " << _STB(_true);
      caller.error(ss.str());
   }
}

template<typename T, typename C>
inline void check_prime(const C & caller, const T val, bool _true = true)
{
   // TODO : faster prime-checking
   bool prime = (val % 2) != 0;
   if (prime)
   {
      const T end = (T)sqrt(val);
      for (T i = 3; prime && i < end; i+=2)
         prime &= (val % i) != 0;
   }
   const bool good = prime; 
   if (!good == _true)
   {
      std::stringstream ss;
      ss << val << " should " << _STBN(_true) << "be prime";
      caller.error(ss.str());
   }
}

template<typename T, typename C>
inline void check_divisibleBy(const C & caller, const T val, T div, bool _true = true)
{
   const bool good = (val % div == 0); 
   if (!good == _true)
   {
      std::stringstream ss;
      ss << val << " is " << _STBN(_true) << "divisible by " << div;
      caller.error(ss.str());
   }
}


/// Base-checker ///
class BaseChecker
{
protected:

   int iRowRead, iColRead;
   void init()
   {
      iRowRead = BuffCheck.getRow();
      iColRead = BuffCheck.getCol();
   }
public:
   static const char NO_END = '\006';

   void clearRowCol()
   {
      iRowRead = iColRead = -1;
   }
};

/// One-checker ///
template <class T> class c : public BaseChecker
{
private:
  T value;
public:
   c<T>(T _val)
   {
      setVal(_val);
   }
   
   operator T() const
   {
      return value;
   }
   
   T& data()
   {
      return value;
   }   

   // Read from stdin
   inline static c<T> read(char end = '\n')
   {
      c<T> res;
      res.setVal( readVal() );
      if (end != NO_END)
         BuffCheck.getChar(end);
      return res;
   }

   template< class U = T, class = typename std::enable_if<std::is_same<U, char>::value>::type >
   inline static c<T> readNewLine()
   {
      return c<char>::read(NO_END).equal('\n');
   }

   /// Generic call mechanism for Functors ///
   template< typename Function >  
   const c<T> & call(Function function, bool _true = true) const
   {
      function(*this, value, _true);
      return *this;
   }   

   /// Common to all ///
   const c<T> & equal(const T valEq, bool _true = true) const
   {
      check_equal(*this, value, valEq, _true);
      return *this;
   }
   const c<T> & greater(const T minVal, bool _true = true) const
   {
      check_greater(*this, value, minVal, _true);
      return *this;
   }
   const c<T> & smaller(const T maxVal, bool _true = true) const
   {
      check_smaller(*this, value, maxVal, _true);
      return *this;
   }
   const c<T> & in(const T minVal, const T maxVal, bool _true = true) const
   {
      check_in(*this, value, minVal, maxVal, _true);
      return *this;
   }
   template< typename U>
   const c<T> & in(U values, bool _true = true) const
   {
      check_in(*this, value, values, _true);
      return *this;
   }      
   

   /// Only for integers ///
   template< class U = T, class = typename std::enable_if<std::is_integral<U>::value>::type >
   const c<U> & prime(bool _true = true) const
   {
      check_prime(*this, value, _true);
      return *this;
   }
   template< class U = T, class = typename std::enable_if<std::is_integral<U>::value>::type >
   const c<U> & divisibleBy(const U div, bool _true = true) const
   {
      check_divisibleBy(*this, value, div, _true);
      return *this;
   }
   
public:
   void error(std::string fmt, ...) const
   {
      va_list ap;
      va_start(ap, fmt);
      std::string s = format("Class %s started on (Lig %3d, Col %3d)\n\tERROR: ", this->getName().c_str(), iRowRead, iColRead);
      verror(s + fmt + "\n", ap);
      va_end(ap); 
   }  
   
   /// Reading functions
   template< class U = T, typename std::enable_if<std::is_same<U, int>::value, int>::type = 0>
   inline static U readVal()
   {
      return BuffCheck.getInt();
   }
   template< class U = T, typename std::enable_if<std::is_same<U, double>::value, int>::type = 0>
   inline static U readVal()
   {
      return BuffCheck.getDouble();
   }
   template< class U = T, typename std::enable_if<std::is_same<U, char>::value, int>::type = 0>
   inline static U readVal()
   {
      return BuffCheck.getChar();
   }
private:
   c<T>()
   {
      init();
   }
   
   void setVal(T val)
   {
      value = val;
   }
   
   std::string getName() const
   {
      return format("c<%s>", typeid(T).name());
   }   
};
typedef c<int> cInt;
typedef c<double> cDouble;
typedef c<char> cChar;


/// Line-checker ///
template <class T> class cVector : public BaseChecker
{
private:
   std::vector<T> vec;
   unsigned int iCol;
public:
   cVector<T>(std::vector<T> & V)
   {
      setVal(V);
   }

   template< class U = T, class = typename std::enable_if<std::is_same<U, char>::value>::type >
   cVector<T>(std::basic_string<char> V)
   {
      vec = std::vector<T>(V.begin(), V.end());
   }

   operator std::vector<T>()
   {
      return vec;
   }

   template< class U = T, class = typename std::enable_if<std::is_same<U, char>::value>::type >
   operator std::string()
   {
      std::string res(vec.begin(), vec.end());
      return res;
   }



   std::vector<T> & data()
   {
      return vec;
   }
   
   T operator[](int i) const
   {
      return vec[i];
   }

   // Read from stdin
   static cVector<T> read(int nbVals, char sep = ' ', char end = '\n')
   {
      cVector<T> res;
      res.vec.resize(nbVals);
      for (int i = 0 ; i < nbVals ; i++)
      {
         res.vec[i] = c<T>::readVal();
         if(i != nbVals - 1)
            BuffCheck.getChar(sep);
         else
            BuffCheck.getChar(end);
      }
      return res;
   }

   cVector<T> sub(int start, int length = -1)
   {
      const int size = vec.size();
      if ( !(0 <= start && start < size))
         error("start is '%d' but there is only %d elements", start, size);
      if (length != -1 && start + length > size)
         error("start is '%d', length is '%d' but there is only %d elements", start, length, size);
      if (length == -1)
         length = vec.size() - start;

      cVector<T> res;
      res.vec = std::vector<T>(vec.begin() + start, vec.begin() + start + length);
      return res;
   }

   /// Specific to cVector<char> = cString ///
   template< class U = T, class = typename std::enable_if<std::is_same<U, char>::value>::type >
   c<int> size()
   {
      cInt s = data().size();
      s.clearRowCol();
      return s;
   }
   
   template< class U = T, class = typename std::enable_if<std::is_same<U, char>::value>::type >
   inline static cVector<T> readLine()
   {
      cVector<T> res;
      std::string s = BuffCheck.getLine();
      res.vec.resize(s.size());
      std::copy(s.begin(), s.end(), res.vec.begin());
      return res;
   }

   template< class U = T, class = typename std::enable_if<std::is_same<U, char>::value>::type >
   inline static cVector<T> readWord(char spaceChar)
   {
      cVector<T> res;
      std::string s = BuffCheck.getWord(spaceChar);
      res.vec.resize(s.size());
      std::copy(s.begin(), s.end(), res.vec.begin());
      return res;
   }

   template< class U = T, class = typename std::enable_if<std::is_same<U, char>::value>::type >
   static cVector<T> readChars(int nbChars, char end = '\n')
   {
      cVector<T> res;
      res.vec.resize(nbChars);
      for (int i = 0 ; i < nbChars ; i++)
      {
         res.vec[i] = c<T>::readVal();
      }
      if (end != NO_END)
         BuffCheck.getChar(end);
      return res;
   }
   
   /// Generic call mechanism for Functors ///
   template< typename Function >  
   cVector<T> & call(Function function, bool _true = true) 
   {
      for (iCol = 0; iCol < vec.size(); iCol++)
         function(*this, vec[iCol], _true);
      return *this;
   }
   
   /// Common to all ///
   cVector<T> & equal(const T valEq, bool _true = true)
   {
      for (iCol = 0; iCol < vec.size(); iCol++)
         check_equal(*this, vec[iCol], valEq, _true);
      return *this;
   }   
   cVector<T> & smaller(const T maxVal, bool _true = true)
   {
      for (iCol = 0; iCol < vec.size(); iCol++)
         check_smaller(*this, vec[iCol], maxVal, _true);
      return *this;
   }
   cVector<T> & greater(const T minVal, bool _true = true)
   {
      for (iCol = 0; iCol < vec.size(); iCol++)
         check_greater(*this, vec[iCol], minVal, _true);
      return *this;
   }
   cVector<T> & in(const T minVal, const T maxVal, bool _true = true)
   {
      for (iCol = 0; iCol < vec.size(); iCol++)
         check_in(*this, vec[iCol], minVal, maxVal, _true);
      return *this;
   }   
   template< typename U>
   cVector<T> & in(U values, bool _true = true)
   {
      for (iCol = 0; iCol < vec.size(); iCol++)
         check_in(*this, vec[iCol], values, _true);
      return *this;
   }
   
   
   /// Only for integers ///
   template< class U = T, class = typename std::enable_if<std::is_integral<U>::value>::type >
   cVector<T> & prime(bool _true = true)
   {
      for (iCol = 0; iCol < vec.size(); iCol++)
         check_prime(*this, vec[iCol], _true);
      return *this;
   }
   template< class U = T, class = typename std::enable_if<std::is_integral<U>::value>::type >
   cVector<T> & divisibleBy(const T div, bool _true = true)
   {
      for (iCol = 0; iCol < vec.size(); iCol++)
         check_divisibleBy(*this, vec[iCol], div, _true);
      return *this;
   }
   
   
   /// Specific to vectors ///
   /*
   cVector<T> & sort()
   {
      std::sort(vec.begin(), vec.end());
      return *this;
   }
   */
   // Verify that no element is present twice   
   cVector<T> & unique()
   {
      noCoords();
      std::vector<T> data(vec);
      std::sort(data.begin(), data.end());
      for (unsigned int i = 0; i < data.size() - 1; i++)
      {
         const bool good = (data[i] != data[i+1]); 
         if (!good)
         {
            std::stringstream ss;
            ss << "Unique(True) : the value " << data[i] << " is present at least twice";
            error(ss.str());
         }     
      }
      return *this;
   }
   cVector<T> & constant(bool _true = true)
   {
      for (iCol = 0; iCol < vec.size() - 1; iCol++)
      {
         const bool good = (vec[iCol] == vec[iCol+1]); 
         if (!good == _true)
         {
            std::stringstream ss;
            ss << "Decreasing(" << _STB(_true) << ") : '" << vec[iCol] << " == " << vec[iCol+1] << "' should be " << _STB(_true);
            error(ss.str());
         }     
      }
      return *this;
   }
   cVector<T> & increasing(bool strict = false, bool _true = true)
   {
      for (iCol = 0; iCol < vec.size() - 1; iCol++)
      {
         bool good;
         std::string symbol;
         if (strict)
         {
            good = (vec[iCol] < vec[iCol+1]); 
            symbol = "<";            
         }
         else
         {
            good = (vec[iCol] <= vec[iCol+1]); 
            symbol = "<=";
         }
         if (!good == _true)
         {
            std::stringstream ss;
            ss << "Increasing(" << _STB(_true) << ") : '" << vec[iCol] << " " << symbol << " " << vec[iCol+1] << "' should be " << _STB(_true);
            error(ss.str());
         }     
      }
      return *this;
   }
   cVector<T> & decreasing(bool strict = false, bool _true = true)
   {
      for (iCol = 0; iCol < vec.size() - 1; iCol++)
      {
         bool good;
         std::string symbol;
         if (strict)
         {
            good = (vec[iCol] > vec[iCol+1]); 
            symbol = ">";            
         }
         else
         {
            good = (vec[iCol] >= vec[iCol+1]); 
            symbol = ">=";
         }
         if (!good == _true)
         {
            std::stringstream ss;
            ss << "Decreasing(" << _STB(_true) << ") : '" << vec[iCol] << " " << symbol << " " << vec[iCol+1] << "' should be " << _STB(_true);
            error(ss.str());
         }     
      }
      return *this;
   }

   #if 0
   template< class U = T, class = typename std::enable_if<std::is_same<U, char>::value>::type >
   cVector<T> & match(const std::string regex, bool _true = true)
   {
      noCoords();
      std::regex txt_regex(regex);
      const bool good = std::regex_match(vec.begin(), vec.end(), txt_regex);
      if (!good == _true)
      {
         std::stringstream ss;
         ss << "the text does" << _STBN(_true) << " match the regexp '" << regex << "'";
         error(ss.str());
      }
      return *this;
   }
   #endif
   
public:
   void error(std::string fmt, ...) const
   {
      va_list ap;
      va_start(ap, fmt);
      std::string s = "";
      s += format("Class %s started on (Lig %d, Col %d)\n", this->getName().c_str(), iRowRead, iColRead);
      if (iCol != vec.size())
         s += format("Value at position (%d)\n", iCol + 1);
      s += format("\tERROR: ");
      verror(s + fmt + "\n", ap);
      va_end(ap); 
   }  
private:
   cVector<T>()
   {
      init();
   }
   void setVal(std::vector<T> & V)
   {
      vec.resize(V.size());
      copy(V.begin(), V.end(), vec.begin());
   }
   std::string getName() const
   {
      return format("cVector<%s>(size = %d)", typeid(T).name(), vec.size());
   }   
   void noCoords()
   {
      iCol = vec.size();   
   }   
};
typedef cVector<char> cString;




/// Matrix-checker ///
template <class T> class cMatrix : public BaseChecker
{
private:
   std::vector< std::vector<T> > mat;
   unsigned int iCol, iRow;
public:
   cMatrix<T>(std::vector< std::vector<T> > & V)
   {
      setVal(V);
   }

   operator std::vector< std::vector<T> >()
   {
      return mat;
   }      
   std::vector< std::vector<T> > & data()
   {
      return mat;
   }
   
   // Read from stdin
   static cMatrix<T> read(int nbLines, int nbVals, char sep = ' ', char end = '\n')
   {
      cMatrix<T> res;
      res.mat.resize(nbLines);
      for (int l = 0 ; l < nbLines ; l++)
      {
         /*
         // Old implementation
         res.mat[l].resize(nbVals);
         for (int i = 0 ; i < nbVals ; i++)
         {
            res.mat[l][i] = c<T>::readVal();
            if(i != nbVals - 1)
               BuffCheck.getChar(sep);
            else
               BuffCheck.getChar(end);
         }
         */
         // Short implementation
         std::vector<T> tmp = cVector<T>::read(nbVals, sep, end).data();
         res.mat[l].resize(tmp.size());
         std::copy(tmp.begin(), tmp.end(), res.mat[l].begin()); 
      }
      return res;
   }

   template< class U = T, class = typename std::enable_if<std::is_same<U, char>::value>::type >
   static cMatrix<T> readLines(int nbLines)
   {
      cMatrix<T> res;
      res.mat.resize(nbLines);
      for (int l = 0 ; l < nbLines ; l++)
      {
         std::vector<T> line = cVector<T>::readLine().data();
         res.mat[l].resize(line.size());
         std::copy(line.begin(), line.end(), res.mat[l].begin()); 
      }
      return res;
   }

   /// Generic call mechanism for Functors ///
   template< typename Function >  
   cMatrix<T> & call(Function function, bool _true = true) 
   {
      for (iRow = 0; iRow < mat.size(); iRow++)
      for (iCol = 0; iCol < mat[0].size(); iCol++)
         function(*this, mat[iRow][iCol], _true);
      return *this;
   }
   
   /// Common to all ///
   cMatrix<T> & check_equal(const T minVal, bool _true = true)
   {
      for (iRow = 0; iRow < mat.size(); iRow++)
      for (iCol = 0; iCol < mat[0].size(); iCol++)
         check_equal(*this, mat[iRow][iCol], minVal, _true);
      return *this;
   }
   cMatrix<T> & smaller(const T maxVal, bool _true = true)
   {
      for (iRow = 0; iRow < mat.size(); iRow++)
      for (iCol = 0; iCol < mat[0].size(); iCol++)
         check_smaller(*this, mat[iRow][iCol], maxVal, _true);
      return *this;
   }
   cMatrix<T> & greater(const T minVal, bool _true = true)
   {
      for (iRow = 0; iRow < mat.size(); iRow++)
      for (iCol = 0; iCol < mat[0].size(); iCol++)
         check_greater(*this, mat[iRow][iCol], minVal, _true);
      return *this;
   }
   cMatrix<T> & in(const T minVal, const T maxVal, bool _true = true)
   {
      for (iRow = 0; iRow < mat.size(); iRow++)
      for (iCol = 0; iCol < mat[0].size(); iCol++)
         check_in(*this, mat[iRow][iCol], minVal, maxVal, _true);
      return *this;
   }
   template< typename U>
   cMatrix<T> & in(U values, bool _true = true)
   {
      for (iRow = 0; iRow < mat.size(); iRow++)
      for (iCol = 0; iCol < mat[0].size(); iCol++)
         check_in(*this, mat[iRow][iCol], values, _true);
      return *this;
   }
   
   /// Only for integers ///
   template< class U = T, class = typename std::enable_if<std::is_integral<U>::value>::type >
   cMatrix<T> & prime(bool _true = true)
   {
      for (iRow = 0; iRow < mat.size(); iRow++)
      for (iCol = 0; iCol < mat[0].size(); iCol++)
         check_prime(*this, mat[iRow][iCol], _true);
      return *this;
   }
   template< class U = T, class = typename std::enable_if<std::is_integral<U>::value>::type >
   cMatrix<T> & divisibleBy(const T div, bool _true = true)
   {
      for (iRow = 0; iRow < mat.size(); iRow++)
      for (iCol = 0; iCol < mat[0].size(); iCol++)
         check_divisibleBy(*this, mat[iRow][iCol], div, _true);
      return *this;
   }
   
   
public:
   std::string getName() const
   {
      return format("cMatrix<%s>(size = %dx%d)", typeid(T).name(), mat.size(), mat[0].size());
   }
   void error(std::string fmt, ...) const
   {
      va_list ap;
      va_start(ap, fmt);
      std::string s = "";
      s += format("Class %s started on (Lig %d, Col %d)\n", this->getName().c_str(), iRowRead, iColRead);
      if (iRow != mat.size() || iCol != mat[0].size())
         s += format("Value at position (%d, %d)\n", iRow + 1, iCol + 1);
      s += format("\tERROR: ");
      verror(s + fmt + "\n", ap);
      va_end(ap); 
   }  
private:
   cMatrix<T>()
   {
      init();
   }
   void setVal(std::vector< std::vector<T> > & V)
   {
      mat.resize(V.size());
      for (unsigned int i = 0 ; i < V.size() ; i++)
      {
         mat[i].resize(V[0].size());
         copy(V[i].begin(), V[i].end(), mat[i].begin());
      }
   }
   void noCoords()
   {
      iRow = mat.size();
      iCol = mat[0].size();
   }   
   
};


namespace sani
{
   // Useful string constants
   std::string UPPER       = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   std::string LOWER       = "abcdefghijklmnopqrstuvwxyz";
   std::string DIGITS      = "0123456789";
   std::string ALPHA       = LOWER + UPPER;
   std::string ALPHANUM    = ALPHA + DIGITS;

   // Error function
   void error(std::string fmt, ...)
   {
      va_list ap;
      va_start(ap, fmt);
      verror(fmt + "\n", ap);
      va_end(ap); 
   }  

   bool isFinished()
   {
      return BuffCheck.isFinished();
   }
}


#define SANITIZER_H
#endif

/*
TODO : 
- regexp
- pass the buffer in static
- translating messages ? later
- test writing speed...
*/


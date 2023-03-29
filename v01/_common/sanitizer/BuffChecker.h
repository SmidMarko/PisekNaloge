#ifndef BUFFCHECKER_H
#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

// Show all the details
#ifndef DEBUG_INFOS
   #define DEBUG_INFOS 0
#endif
// Write corrected input
#ifndef WRITE_CORRECTED
   #define WRITE_CORRECTED 0
#endif


// Interface for a buffer
class BuffChecker
{
public:
   static const char NO_CHECK = '\006'; // ACK, should never happened

   virtual char   getChar(char c = BuffChecker::NO_CHECK) = 0;
   virtual int    getInt() = 0;
   virtual double getDouble() = 0;
   
   virtual void   getSpace()
   {
      getChar(' ');
   }
   virtual void   getNewline()
   {
      getChar('\n');
   }

   virtual int getCol() = 0;
   virtual int getRow() = 0;

};


class LBuffer : public BuffChecker
{
  static const int Tm = 4096 ;
  char buf[Tm+2] ;
  int deb ;
   int row ;
   int col ;
  
public:
      inline int getCol()
   {
      return col;
   }
   inline int getRow()
   {
      return row;
   }
   
  inline  char read()
  {
    if( !buf[deb] )
      {
        deb = 0 ;
        buf[0] = buf[1] = 0 ; //for EOF case
        if( fgets(buf, 100, stdin) != NULL )
          {
            if( buf[strlen(buf)-1] == '\n' )
              {
                row++ ;
                col = 0 ;
              }
            else
              col += Tm ;
          }
      }
    return buf[deb] ;
  }

  inline  void fail( const char * c = NULL)
  {
    fprintf(stderr,"Erreur à la ligne %d, colonne %d",row,col+deb);
    if( c )
      fprintf(stderr,"%s", c);
    fprintf(stderr,"\n");
    exit(1) ;
  }
  

  inline  char pop()
  {
    char c = read();
    deb++;
    return c ;
  }

  inline   char getChar(char c = ' ')
  {
   char ch = pop();
    if( ch != c )
    {

      fail("Wrong char") ;
    }
return ch;
  }

  
  inline  int getInt()
   {
   const int Tm = 12 ;
     char num[Tm+2] ;
     int t = 0 ;
     bool sign  =  read() == '-' ;
       
     if( sign)
       pop();
       
     while( '0' <= read() && read() <= '9' && t < Tm)
       num[t++] = pop();
     num[t] = '\0' ; 
     // 2^31 = 2 147 483 648 < 10^10
   if(t == 0)
      fail("could not read int");

     long long res = 0 ;
     t = 0 ;
     while( num[t] )
       {
         res = num[t]-'0' + res * 10LL ;
         t++ ;
       }
     

     if( t > 10 ||  res < (long long)INT_MIN || res > (long long)INT_MAX )
       fail("dépassement des capacités d'un entier 32-bits");

     return (int)res;
   }
  inline  double getDouble()
  {
     const int Tm = 42 ;
     char num[Tm+2] ; 
     int t = 0 ;
       
     if( read() == '-' )
       num[t++] = pop();
     
     while( '0' <= read() && read() <= '9' && t < Tm )
       num[t++] = pop() ;
     
     if( read() == '.' )
       do
         num[t++] = pop() ;
       while( '0' <= read() && read() <= '9' && t < Tm ) ;
     if(t == 0)
      fail("could not read double");


     num[t++] = '\0' ;
     double v ;
     sscanf(num,"%lf",&v);


     return v;
   }
  
};


bool SANI_EXIT = false;

// We can do it more efficiently latter if we want
// Utility function
std::string format(std::string fmt, ...)
{
   char buf[10 * 1000];
   va_list aq;
   va_start(aq, fmt);
   vsprintf(buf, fmt.c_str(), aq);
   va_end(aq);
   return std::string(buf);
}
// Same one, with va_args
std::string vformat(std::string fmt, va_list args) {
   char buf[10 * 1000];
   vsprintf(buf, fmt.c_str(), args);
   return std::string(buf);
}

void error(std::string fmt, ...)
{
   va_list ap;
   va_start(ap, fmt);
   fprintf(stderr, "%s", vformat(fmt, ap).c_str());
   va_end(ap); 
   SANI_EXIT = true;
   exit(1);
}
void verror(std::string fmt, va_list args)
{
   fprintf(stderr, "%s", vformat(fmt, args).c_str());
   SANI_EXIT = true;
   exit(1);
}

// http://www.cplusplus.com/reference/clibrary/cstdlib/atexit/ ??

// We might not be able to correctly check the limit values but that's ok
// MaxVal = 2147483639 instead of 2147483647 
class FastIO : public BuffChecker
{
protected:
   const static int SIZE_BUF = 4096;
   const static int MY_INT_MAX = 214748363; // floor( (2 ** 31 - 1 - 9) / 10 )
   char * nc, * lc; // next char / last char
   char buff[SIZE_BUF];
   int iRow, iCol;

public:
   FastIO() 
   { 
      iRow = iCol = 1;
      nc = lc = buff; 
      read(); 
   };
   ~FastIO() 
   {
      if (!SANI_EXIT)
         finished();
   }
   
   
   
   inline int getCol()
   {
      return iCol;
   }
   inline int getRow()
   {
      return iRow;
   }
   #define getDump(name)\
      char name[50];\
      strncpy(name, pOri, nc + 1 - pOri);\
      name[nc + 1 - pOri] = '\0';
   
   void error(std::string fmt, ...)
   {
      va_list ap;
      va_start(ap, fmt);
      std::string s = format("[Lig %3d, Col %3d] ERROR: ", iRow, iCol);
      verror(s + fmt + "\n", ap);
      va_end(ap); 
   }

   void debug(std::string fmt, ...)
   {
      va_list ap;
      va_start(ap, fmt);
      std::string s = format("[Lig %3d, Col %3d] debug: ", iRow, iCol);
      s = vformat(s + fmt + "\n", ap);
      va_end(ap); 
      fprintf(stderr, "%s", s.c_str());
   }


   std::string getLine()
   {
      std::string s;
      while (true)
      {
         const char c = getChar();
         if (c != '\n')
            s += c;
         else
            return s;
      }
   }

   std::string getWord(char end = ' ')
   {
      std::string s;
      while (true)
      {
         const char c = getChar();
         if (c != ' ' && c != '\t' && c != '\n')
            s += c;
         else
         {
            if (c != end)
               error("Wrong char : '\\%03d' instead of '\\%03d'\n '%c' vs '%c'", c, end, c, end);
            return s;
         }
      }
   }

   
   // Reads a caracter and, if c != BuffChecker::NO_CHECK, checks if it equals c
   char getChar(char c = BuffChecker::NO_CHECK)
   {
      // Init stuff
      this->fillBuffer();
      //const char* pOri = nc;
      
      if (nc > lc)
         error("End of input reached");

      #if DEBUG_INFOS
         debug("getChar()");
      #endif
      #if WRITE_CORRECTED
         // Eat additionnal spaces
         if (c == '\n')
            while (*nc == ' ')
               nc++;
      #endif

      // Read the number
      const char ch = *nc++;
      if (c != BuffChecker::NO_CHECK && c != ch)
         error("Wrong char : '\\%03d' instead of '\\%03d'\n '%c' vs '%c'", ch, c, ch, c);

      // Update variables
      iCol++;
      if (ch == '\n')
      {
         iRow++;
         iCol = 1;
      }

      #if DEBUG_INFOS
         debug("'%c'", ch);
      #endif
      #if WRITE_CORRECTED
         fputc(c, stdout);
         // Eat additionnal spaces
         while (*nc == ' ')
            nc++;
      #endif

      return ch;
   }

   // Reads an integer
   int getInt()
   {
      // Init stuff
      this->fillBuffer();
      const char* pOri = nc;

      #if DEBUG_INFOS
         debug("getInt()");
      #endif

      // Read the number
      int buf = 0, sign = 1;
      if (*nc == '-')
      {
         sign = -1;
         nc++;
      }
      while('0' <= *nc && *nc <= '9')
      {
         if (buf > MY_INT_MAX)
         {
            getDump(tmp);
            error("The integer '%s...' is too big", tmp);
         }
         buf = buf * 10 + (*nc++ - '0');
      }

      // Nothing was read
      if (nc == pOri)
      {
         getDump(tmp);        
         error("'%s...' is not the beginning of an integer", tmp);
      }

      // Update variables
      iCol += nc - pOri;

      #if DEBUG_INFOS
         debug("'%d'", sign * buf);
      #endif
      #if WRITE_CORRECTED
         fwrite(pOri, 1, nc - pOri, stdout);
      #endif

      return sign * buf;
   }

   double getDouble()
   {
      // Init stuff
      this->fillBuffer();
      const char* pOri = nc;

      #if DEBUG_INFOS
         debug("getDouble()");
      #endif

      // Read the number
      double buf = 0, sign = 1;
      int nbADP = 0;
      bool tooBig = false;
      if (*nc == '-')
      {
         sign = -1;
         nc++;
      }
      while ('0' <= *nc  && *nc <= '9')
         buf = buf * 10 + (*nc++ - '0');
      tooBig |= (nc - pOri > 15);
      if (*nc == '.')
      {
         nc++;
         while('0' <= *nc  && *nc <= '9')
         {
            buf = buf * 10 + (*nc++ - '0');
            nbADP++;
         }
      }
      tooBig |= (nc - pOri > 16);
      if (tooBig)
      {
         if (*nc == '\n') nc--;
         getDump(tmp);
         error("The double '%s...' is too big", tmp);
      }

      while (nbADP--)
         buf /= 10.0;

      // Nothing was read
      if (nc == pOri)
      {
         getDump(tmp);
         error("'%s...' is not the beginning of a double", tmp);
      }

      // Update variables
      iCol += nc - pOri;

      #if DEBUG_INFOS
         debug("'%lf'", (double)(sign * buf));
      #endif
      #if WRITE_CORRECTED
         fwrite(pOri, 1, nc - pOri, stdout);
      #endif

      return sign * buf;
   }

   bool isFinished()
   {
      return (nc == lc);
   }
   // Will produce an error if the EOF was not reached during the reading
   void finished()
   {
      #if WRITE_CORRECTED
         return;
      #endif
      if (nc != lc)
      {
         std::string msg;
         for (char* p = nc; p != lc ; p++)
            msg += format("\\%03d", *p);
         msg += "\n";
         for (char* p = nc; p != lc ; p++)
            msg += format("%c", *p);
         msg += "\n";
         error("There is still data to read.\nWe are at '%p', data continue until '%p' at least :\n%s", nc, lc, msg.c_str());
      }
   }
   void fail(std::string s)
   {
      error((s+'\n').c_str());
   }

protected:
   // Reads 'size' octets from stdin and puts it in the buffer
   inline void read(int size = SIZE_BUF)
   {
      //printf("read : lc=%p\n", lc);
      const int nbChars = fread(lc, 1, size, stdin);
      lc = lc + nbChars;
      //printf("read : lc=%p, nbChars=%d\n", lc, nbChars);
   }

   // Fills the buffer as much as possible
   inline void fillBuffer(bool force = false)
   {
      if(force || nc - buff > SIZE_BUF - 50) // 50 : enough for everything
      {
         //printf("fill : nc=%p, buff=%p\n", nc, buff);
         const int s = buff + SIZE_BUF - nc;
         memcpy(buff, nc, sizeof(char)*s);
         //printf("fill : lc=%p, lc=%p\n", lc, buff + s);
         // Push it to the left by (nc-buff)
         lc = lc - (nc-buff);
         nc = buff;
         this->read(SIZE_BUF - s);
      }
   }
};

#define BUFFCHECKER_H
#endif

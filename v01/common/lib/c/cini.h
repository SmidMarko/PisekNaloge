#ifndef _CINI_H_
#define _CINI_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

typedef char* string;

/*
inline void CINI_print_bool(bool b)
{
  if (b == false)
    printf("false");
  else
    printf("true");
}
inline void CINI_print_int(int n)      {printf("%d", n);}
inline void CINI_print_float(float f)  {printf("%f", f);}
inline void CINI_print_double(double lf)  {printf("%lf", lf);}
inline void CINI_print_string(char *s) {printf("%s", s);}
inline void CINI_print_char(char c)    {printf("%c", c);}
inline void CINI_newline()             {printf("\n");}

inline int CINI_read_int()
{
   int d;
   if (scanf("%d", &d))
      return d;
   fprintf(stderr,"CINI: Erreur dans la lecture du nombre\n");
   exit(1);
}
inline float CINI_read_float()
{
   float f;
   if (scanf("%f", &f))
      return f;
   fprintf(stderr,"CINI: Erreur dans la lecture du nombre\n");
   exit(1);
}
inline double CINI_read_double()
{
   double lf;
   if (scanf("%lf", &lf))
      return lf;
   fprintf(stderr,"CINI: Erreur dans la lecture du nombre\n");
   exit(1);
}
inline char CINI_read_char()
{
   char c;
   if (scanf("%c", &c))
      return c;
   fprintf(stderr,"CINI: Erreur dans la lecture du caractere\n");
   exit(1);
}
*/
#define CINI_was_here()							\
  ({									\
    if(strcmp(__FUNCTION__,""))						\
      fprintf(stderr,							\
	      "Debug: appel à partir de ligne %d, %s (fonction '%s').\n", \
	      __LINE__, __FILE__, __FUNCTION__);			\
    else                                                                \
      fprintf(stderr,							\
	      "Debug: appel à partir de la ligne %d du fichier %s.",	\
	      __LINE__, __FILE__);					\
  })


#define CINI_round(x) ((x)>=0?(int)((x)+0.5):(int)((x)-0.5))

#endif

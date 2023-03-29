#include "multi_lang.h"


int main()
{   
   Var MAX("MAX_TAILLE");
   Var nb("nb"), lim("limite");
   Var pas("pas");
   Var iCase("iParcelle");
   Var tab("tab");
   Var str1("str1", cSTRING), str2("str2", cSTRING);
   Var f("f", cBOOL);

   Noeud p = 
   Prog
   (
      ArrayDecl(nb,100),
      Main
      (
         IntToChar(CharToInt(Char('a')) + nb)
      )
   );

   PrintAll(p);
}



#ifndef MULTI_LANG_CF_H
#define MULTI_LANG_CF_H

#include "multi_lang_base.h"
#include "multi_lang_expr.h"
#include "multi_lang_op.h"
#include "multi_lang_io.h"

// LibString

class nLibString : public Noeud
{
   string str;
   public :
   nLibString(int _id, string _str) { this->id = _id; str = _str;  }

   virtual void Print(int lang, int level = 0)
   {
      if(lang == cJAVA) 
      {
         string tmp = str;
         if(tmp[0] >= 'a' && tmp[0] <= 'z')
            tmp[0] -= 32;
         cout << tmp;
      }
      else
         cout << str;
   }

};

Noeud LibString(string str)
{
   int id = nbNoeuds++;
   noeud[id] = new nLibString(id, str);
   return *noeud[id];
}

void ajouteLib(int id) {}

template<typename ...Args>
void ajouteLib(int id, string str, Args... args)
{
   fils[id].push_back(LibString(str).id);
   ajouteLib(id, args...);
}


// OnlyLib

const int cONLY_LIB = 1383, cADD_LIB = 3738;

class nLib : public Noeud
{
   public :

   nLib(int _id, int _type) { this->id = _id; this->type = _type; }

   virtual void Print(int lang, int level = 0)
   {

      for(auto idFils : fils[this->id])
      {
         if(lang == cCPP || lang == cC)
         {
            cout << "#include \"";
            noeud[idFils]->Print(lang,level);
            cout << ".h\"" << endl;
         }
         if(lang == cPYTHON)
         {
            cout << "from ";
            noeud[idFils]->Print(lang,level);
            cout << " import *" << endl;
         }
         if(lang == cJAVA)
         {
            cout << "import static algorea.";
            noeud[idFils]->Print(lang,level);
            cout << ".*;" << endl;
         }
         
      }

      if(lang == cCPP || lang == cC || lang == cPYTHON ||lang == cJAVA)
         cout << endl;
   }
};

template<typename ...Args>
Noeud OnlyLib(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nLib(id, cONLY_LIB);
   ajouteLib(id, args...);
   return *noeud[id];
}

template<typename ...Args>
Noeud AddLib(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nLib(id, cADD_LIB);
   ajouteLib(id, args...);
   return *noeud[id];
}

// Prog

class nProg : public Noeud
{
   public :

   nProg(int _id) { this->id = _id; }

   virtual void PrintSeparator(int lang) {}

   virtual void Print(int lang, int level = 0)
   {  
      bool modeOnlyLib = false, modeAddLib = false;
      if((int)(fils[this->id].size()) > 0)
      {
         modeOnlyLib = noeud[fils[this->id][0]]->type == cONLY_LIB;
         modeAddLib  = noeud[fils[this->id][0]]->type == cADD_LIB;
      }

      if(lang == cCPP)
      {
         if(!modeOnlyLib)
         {
            cout << "#include <iostream>" << endl  
                 << "#include <algorithm>" << endl;
            if(containsLib(this->id, lSTRING))
               cout << "#include <string>" << endl;
         }  
         if(modeOnlyLib || modeAddLib)
            noeud[fils[this->id][0]]->Print(lang);
         if(!modeOnlyLib)
            cout << "using namespace std;" << endl << endl;      
      }
      if(lang == cC)
      {
         if(!modeOnlyLib)
         {
            cout << "#include <stdio.h>" << endl;
            if(containsLib(this->id, lBOOL)) 
               cout << "#include <stdbool.h>" << endl;  
            if(containsLib(this->id, lSTRING))
               cout << "#include <string.h>" << endl;
         } 
         if(modeOnlyLib || modeAddLib)
            noeud[fils[this->id][0]]->Print(lang);
         else
            cout << endl; 
      }
      if(lang == cJAVA)
      {
         if(actif[LIB.id])
            cout << "package algorea;" << endl;
         if(!modeOnlyLib && !actif[NO_SCANNER.id])
            cout << "import algorea.Scanner;" << endl;   
         if(modeOnlyLib || modeAddLib)
            noeud[fils[this->id][0]]->Print(lang); 
         else
            cout << endl;

      }
      if(lang == cPYTHON)
      {
         if(modeOnlyLib || modeAddLib)
            noeud[fils[this->id][0]]->Print(lang); 
      }     

      int lev = 0;
      if(lang == cJAVA)
         lev = 1;

      if(lang == cJAVA)
      {
         if(actif[LIB.id])
            cout << "public class " << lib_name << endl << "{" << endl;
         else
            cout << "class Main" << endl << "{" << endl;
         if(lang == cJAVA && containsLib(this->id, lREAD))
         {
            espaces(level+1);
            cout << "static Scanner input;" << endl << endl;
            activeModes(USE_READ);
         }
      }
      for(int iFils = 0; iFils < (int)(fils[this->id].size()); iFils++)
         if(!(modeOnlyLib || modeAddLib) || iFils > 0)
         {
            int idFils = fils[this->id][iFils];
            noeud[idFils]->Print(lang,lev);
            noeud[idFils]->PrintSeparator(lang);
         }
      if(lang == cJAVA)
         cout << "}";
      cout << endl;
      desactiveModes(USE_READ);
   }
};

template<typename ...Args>
Noeud Prog(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nProg(id);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

// Groupe

class nGroup : public Noeud
{
   public :

   nGroup(int _id) {this->id = _id; }

   virtual void PrintSeparator(int lang) { }

   virtual bool MultiLines(int lang){ return false; }

   virtual void Print(int lang, int level = 0)
   {
      for(int idFils : fils[this->id])
      {
         noeud[idFils]->Print(lang, level);
         noeud[idFils]->PrintSeparator(lang);
      }
   }
};

template<typename ...Args>
Noeud Group(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nGroup(id);
   nBloc B(id);
   B(args...);
   return *noeud[id];
}

// IfMode

class nIfMode : public Noeud
{
   int idMode;   

   public :

   nIfMode(int _id, int _idMode) 
   { 
      this->id = _id; 
      idMode = _idMode;
   }

   virtual void PrintSeparator(int lang) {}

   virtual void SetDefReadVariable(bool val) 
   {
      if(_mode[idMode]->isSatisfied())
         for(int idFils : fils[this->id])
            noeud[idFils]->SetDefReadVariable(val);
   }

   virtual void Print(int lang, int level = 0)
   {
      if(_mode[idMode]->isSatisfied())
         for(int idFils : fils[this->id])
         {
            noeud[idFils]->Print(lang, level);
            noeud[idFils]->PrintSeparator(lang);
         }
   }
};

nBloc IF(Mode m)
{
   int id = nbNoeuds++;
   noeud[id] = new nIfMode(id, m.id);
   return nBloc(id);
}

// Encapsuleur

class Encapsuleur : public Noeud
{
   public :

   bool accoladesFac; 

   Encapsuleur() { accoladesFac = false; }
   
   virtual void PrintHead(int lang) { cout << "PrintHead pas defini" << endl; }
   
   virtual void PrintSeparator(int lang) { }

   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      PrintHead(lang);
      bool condAcc = 
         (lang == cCPP || lang == cC || lang == cJAVA || lang == cJVS) && 
            (!accoladesFac || (int)(fils[this->id].size()) > 1 
                || ( (int)(fils[this->id].size()) == 1 
                     && noeud[fils[this->id][0]]->MultiLines(lang)) );

      if(condAcc)
      {
         espaces(level);
         cout << "{" << endl;
      }
      for(int idFils : fils[this->id])
      {
         noeud[idFils]->Print(lang, level+1);
         noeud[idFils]->PrintSeparator(lang);
      }
      if(condAcc)
      {
         espaces(level);
         cout << "}" << endl;
      }
   }
};

// Break

class nBreak : public Noeud
{
   public :

   nBreak(int _id) { this->id = _id; }

   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      cout << "break";
   }
};

Noeud Break()
{
   int id = nbNoeuds++;
   noeud[id] = new nBreak(id);
   return *noeud[id];
}

// Return

class nReturn : public Noeud
{
   public:
   int idArg;
   nReturn(int _id, int _idArg) 
   {
      this->id = _id; idArg = _idArg; 
      autresFils[this->id].push_back(idArg);
   }

   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      cout << "return";
      if((int)(fils[idArg].size()) > 0)
         cout << " ";
      noeud[idArg]->Print(lang);
   }
   
   virtual void PrintSeparator(int lang)
   {
      if(lang == cCPP || lang == cC || lang == cJAVA 
         || lang == cJVS)
         cout << ";" << endl;
      if(lang == cPYTHON)
         cout << endl;
   }
};

template<typename ...Args>
Noeud Return(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nReturn(id, Text(args...).id);
   return *noeud[id];
}

// Global

class nGlobal : public Noeud
{
   public:
   nGlobal(int _id) { this->id = _id; }

   virtual void Print(int lang, int level = 0)
   {
      if(lang == cPYTHON)
      {
         espaces(level);
         cout << "global ";
         bool prem = true;
         for(int idFils : fils[this->id])
         {
            if(!prem)
               cout << ", ";
            prem = false;
            noeud[idFils]->Print(lang);
         }
         cout << endl;         
      }
   }
   
   virtual void PrintSeparator(int lang) {}
};

template<typename ...Args>
Noeud Global(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nGlobal(id);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

// Main

class nMain : public Noeud
{
   public :
   nMain(int _id) { this->id = _id; }

   virtual void Print(int lang, int level = 0) 
   {
      if(lang != cPYTHON)
         espaces(level);
      if(lang == cCPP || lang == cC)  
         cout << "int main()" << endl;
      if(lang == cJVS)
         cout << "void main()" << endl; 
      if(lang == cJAVA)
         cout << "static public void main(String args[])" << endl;

      if(lang == cPYTHON) 
         for(int idFils : fils[this->id])
         {
            noeud[idFils]->Print(lang, level);
            noeud[idFils]->PrintSeparator(lang);
         }         
      else
      {
         espaces(level); cout << "{" << endl;          

         if(lang == cJAVA && USE_READ.isSatisfied())
         {
            espaces(level+1);
            cout << "input = new Scanner(System.in);" << endl;
         }

         for(int idFils : fils[this->id])
         {
            noeud[idFils]->Print(lang, level+1);
            noeud[idFils]->PrintSeparator(lang);
         }  
         if(lang == cC || lang == cCPP)
         {
            espaces(level+1);
            cout << endl;
            espaces(level+1); 
            cout << "return 0;" << endl;
         }
         espaces(level); cout << "}";          
      }
      
   }

   virtual void PrintSeparator(int lang) { cout << endl; }

};

template<typename ... Args>
Noeud Main(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nMain(id);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

// Func


class nFunction : public Encapsuleur
{
   int idHeadFunc;

   public :

   nFunction(int _id, int _idHeadFunc)
   {
      this->id = _id;
      idHeadFunc = _idHeadFunc;
   }   

   virtual void PrintHead(int lang)
   {
      noeud[idHeadFunc]->Print(lang);
      cout << endl;
   }
};

class HeadFunc : public Noeud
{
   int idFunc;

   public :

   HeadFunc(int _id, int _idFunc) { this->id = _id; idFunc = _idFunc; }

   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      if(lang == cJAVA) {
         if(actif[LIB.id])
            cout << "public ";
         cout << "static ";
      }
      if(lang == cPYTHON)
         cout << "def ";
      if(lang != cPYTHON)
      {
         PrintType(lang, noeud[idFunc]->type);
         cout << " ";
      }
      noeud[idFunc]->Print(lang);
      cout << "(";
      bool prem = true;
      for(int idFils : fils[this->id])
      {
         if(!prem)
            cout << ", ";
         prem = false;
         if(lang != cPYTHON)
         {
            PrintType(lang, noeud[idFils]->type);
            cout << " ";
         }
         noeud[idFils]->Print(lang);
      }
      cout << ")";
      if(lang == cPYTHON)
         cout << ":";
   }

   template<typename ...Args>
   nBloc operator() (Args... args)
   {
      int idNew = nbNoeuds++;
      noeud[idNew] = new nFunction(idNew, this->id);
      nBloc A(this->id);
      A(args...);
      nBloc B(idNew);
      return B;
   }
};

HeadFunc Function(Noeud f)
{
   int id = nbNoeuds++;
   HeadFunc* nf = new HeadFunc(id, f.id);
   noeud[id] = nf;
   return *nf;
}


// If

class nIf : public Encapsuleur
{
   public :
   int idNoeudCond;
   nIf(int _id, int _idNoeudCond) 
   { this->accoladesFac = true;
     this->id = _id; idNoeudCond = _idNoeudCond; 
     autresFils[this->id].push_back(idNoeudCond);}
   
   virtual void PrintHead(int lang)
   {
      cout << "if";
      if(lang != cPYTHON)
         cout << "(";
      else
         cout << " ";
      noeud[idNoeudCond]->Print(lang);
      if(lang != cPYTHON)
         cout << ")" << endl;
      else
         cout << ":" << endl;
   }
};

template<typename ...Args>
nBloc If(Args... args)
{
   Noeud cond = Text(args...);
   int id = nbNoeuds++;
   noeud[id] = new nIf(id, cond.id);
   return  Bloc(id);
}

// ElseIf

class nElseIf : public Encapsuleur
{
   public :
   int idNoeudCond;
   nElseIf(int _id, int _idNoeudCond) 
   { this->accoladesFac = true;
     this->id = _id; idNoeudCond = _idNoeudCond; 
     autresFils[this->id].push_back(idNoeudCond);}
   
   virtual void PrintHead(int lang)
   {
      if(lang != cPYTHON)
         cout << "else if(";
      else
         cout << "elif ";
      noeud[idNoeudCond]->Print(lang);
      if(lang != cPYTHON)
         cout << ")" << endl;
      else
         cout << ":" << endl;
   }
};

template<typename ...Args>
nBloc ElseIf(Args... args)
{
   Noeud cond = Text(args...);
   int id = nbNoeuds++;
   noeud[id] = new nElseIf(id, cond.id);
   return  Bloc(id);
}

// While

class nWhile : public Encapsuleur
{
   public :
   int idNoeudCond;
   nWhile(int _id, int _idNoeudCond) 
   { this->accoladesFac = true;
     this->id = _id; idNoeudCond = _idNoeudCond; 
     autresFils[this->id].push_back(idNoeudCond); }
   
   virtual void PrintHead(int lang)
   {
      if(lang != cPYTHON)
         cout << "while(";
      else
         cout << "while ";
      noeud[idNoeudCond]->Print(lang);
      if(lang != cPYTHON)
         cout << ")" << endl;
      else
         cout << ":" << endl;
   }
};

template<typename ...Args>
nBloc While(Args... args)
{
   Noeud cond = Text(args...);
   int id = nbNoeuds++;
   noeud[id] = new nWhile(id, cond.id);
   return  Bloc(id);
}

// Else

class nElse : public Encapsuleur
{
   public :
   nElse(int _id) { this->accoladesFac = true; this->id = _id; }
   virtual void PrintHead(int lang) 
   {
      if(lang == cPYTHON) 
         cout << "else:" << endl;
      else 
         cout << "else" << endl;
   }
};

template<typename ... Args>
Noeud Else(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nElse(id);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

// Incr

class nIncr : public Noeud
{
   public :
   bool signePlus;
   int idNoeudInt;
   nIncr(int _id, int _idNoeudInt, bool _signePlus)
   {
      this->id = _id;
      idNoeudInt = _idNoeudInt;
      signePlus = _signePlus;
      autresFils[this->id].push_back(idNoeudInt);
   }

   virtual void Print(int lang, int level = 0)
   {
      if(lang != cPYTHON)
      {
         if(signePlus)
            cout << " + ";
         else
            cout << " - ";
      }  
      else
         if(!signePlus)
            cout << "-";
      noeud[idNoeudInt]->Print(lang);
   }
};

template<typename ...Args>
Noeud Incr(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nIncr(id, Text(args...).id, true);
   return *noeud[id];
}

template<typename ...Args>
Noeud Decr(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nIncr(id, Text(args...).id, false);
   return *noeud[id];
}

// For

////////////////////////////////////////////////////////

class nForRange : public Encapsuleur
{
   public :

   int idIter, idDebut, idFin, idPas;
   bool incr, finExcl, debNul, specPas;

   nForRange(int _id, int _idIter, int _idDebut, int _idFin, int _idPas,
             bool _incr, bool _finExcl, bool _debNul = false, 
             bool _specPas = false)
   {
      this->accoladesFac = true;
      this->id = _id;
      idIter = _idIter;
      idDebut = _idDebut;
      idFin = _idFin;
      idPas = _idPas;
      incr = _incr;
      finExcl = _finExcl;
      debNul = _debNul;
      specPas = _specPas;

      autresFils[this->id].push_back(idIter);
      if(!debNul)
         autresFils[this->id].push_back(idDebut);
       autresFils[this->id].push_back(idFin);
      if(specPas)
         autresFils[this->id].push_back(idPas);
   }

   virtual void PrintHead(int lang)
   {
      if(lang == cC || lang == cCPP || lang == cJAVA || lang == cJVS)
      {
         cout << "for(int ";
         noeud[idIter]->Print(lang);
         cout << " = ";
         if(debNul)
            cout << "0";
         else
            noeud[idDebut]->Print(lang);
         cout << "; ";
         noeud[idIter]->Print(lang);
         if(incr)
            if(finExcl)
               cout << " < ";
            else
               cout << " <= ";
         else
            if(finExcl)
               cout << " > ";
            else
               cout << " >= ";
         
         if(finExcl)
            noeud[idFin]->Print(lang);
         else if((int)(fils[idFin].size()) > 0)
            noeud[fils[idFin][0]]->Print(lang);   
         else
            noeud[idFin]->Print(lang);

         cout << "; ";
         noeud[idIter]->Print(lang);
         cout << " = ";    
         noeud[idIter]->Print(lang);
         noeud[idPas]->Print(lang);
         cout << ")" << endl;
      }
      if(lang == cPYTHON)
      {
         cout << "for ";
         noeud[idIter]->Print(lang);
         cout << " in range(";
         if(debNul)
         {
            if(finExcl)
               noeud[idFin]->Print(lang);
            else if((int)(fils[idFin].size()) > 1)
               noeud[fils[idFin][1]]->Print(lang);
            else
               noeud[idFin]->Print(lang); 
         }
         else
         {
            noeud[idDebut]->Print(lang);
            cout << ", ";

            if(finExcl)
               noeud[idFin]->Print(lang);
            else if((int)(fils[idFin].size()) > 1)
               noeud[fils[idFin][1]]->Print(lang);
            else
               noeud[idFin]->Print(lang);             

            if(specPas)
            {
               cout << ", ";
               noeud[idPas]->Print(lang);
            }
         } 
         cout << "):" << endl;
      }
   }
};

nBloc GenForRange(Noeud iter, Noeud debut, Noeud fin, Noeud pas,
                  bool incr, bool finExcl, bool debNul = false, 
                  bool specPas  = false)
{
   int id = nbNoeuds++;
   noeud[id] = new nForRange(id, iter.id, debut.id, fin.id, pas.id,
                             incr, finExcl, debNul, specPas);
   return nBloc(id);
}

template<typename ...Args>
nBloc Repeat(Noeud iter, Args... args)
{
   return GenForRange(iter, Entier(0), Text(args...), 
                      Incr(1), true, true, true, false);
}

nBloc ForRange(Noeud range, Noeud pas)
{
   int id = range.id;

   if((int)(fils[id].size()) < 2)
      return Repeat(range, 0);

   int idDeb, idIter;
   bool debNul;
   int idFin = fils[id][1];
   if((int)(fils[fils[id][0]].size()) < 2)
   {
      idDeb = id; //bidon
      idIter = fils[id][0];
      debNul = true;
   }
   else
   {
      idDeb = fils[fils[id][0]][0];
      idIter = fils[fils[id][0]][1];
      debNul = false;
   }

   if(range.type == cSUP || range.type == cSUP_STRICT)      
      return GenForRange(*noeud[idIter], *noeud[idDeb], *noeud[idFin], pas,
                          false, range.type == cSUP_STRICT, debNul,true);
   else 
      return GenForRange(*noeud[idIter], *noeud[idDeb], *noeud[idFin], pas,
                          true, range.type == cINF_STRICT, debNul, true);
}

nBloc ForRange(Noeud range)
{
   int id = range.id;

   if((int)(fils[id].size()) < 2)
      return Repeat(range, 0);

   int idDeb, idIter;
   bool debNul;
   int idFin = fils[id][1];
   if((int)(fils[fils[id][0]].size()) < 2)
   {
      idDeb = id; //bidon
      idIter = fils[id][0];
      debNul = true;
   }
   else
   {
      idDeb = fils[fils[id][0]][0];
      idIter = fils[fils[id][0]][1];
      debNul = false;
   }

   if(range.type == cSUP || range.type == cSUP_STRICT)      
      return GenForRange(*noeud[idIter], *noeud[idDeb], *noeud[idFin], Decr(1),
                          false, range.type == cSUP_STRICT, debNul,true);
   else 
      return GenForRange(*noeud[idIter], *noeud[idDeb], *noeud[idFin], Incr(1),
                          true, range.type == cINF_STRICT, debNul, false);
}


#endif

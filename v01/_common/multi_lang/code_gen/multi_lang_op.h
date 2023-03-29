#ifndef MULTI_LANG_OP_H
#define MULTI_LANG_OP_H

#include "multi_lang_base.h"

// Parenthese

class nPar : public Noeud
{
   public :
   int idNoeudInt;
   nPar(int _id, int _idNoeudInt) 
   { 
      this->id = _id; 
      idNoeudInt = _idNoeudInt; 
      autresFils[this->id].push_back(idNoeudInt);
   } 
  
   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      cout << "(";
      noeud[idNoeudInt]->Print(lang);
      cout << ")";
   }
};

template<typename ...Args>
Noeud Par(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nPar(id, Text(args...).id);
   return *noeud[id];
}

// Minus

class nMinus : public Noeud
{
   public :
   int idNoeudInt;
   nMinus(int _id, int _idNoeudInt) 
   { this->id = _id; 
     idNoeudInt = _idNoeudInt; autresFils[this->id].push_back(idNoeudInt);} 
  
   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      cout << "-";
      noeud[idNoeudInt]->Print(lang);
   }
};

Noeud Minus(Noeud n)
{
   int id = nbNoeuds++;
   noeud[id] = new nMinus(id, n.id);
   return *noeud[id];
}

Noeud operator - (Noeud n)
{
   return Minus(n);
}

// Neg

class nNeg : public Noeud
{
   public :
   int idNoeudInt;
   nNeg(int _id, int _idNoeudInt) 
   { this->id = _id; idNoeudInt = _idNoeudInt; 
     autresFils[this->id].push_back(idNoeudInt);} 
  
   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      if(lang == cPYTHON)
      {
         cout << "(not ";
         noeud[idNoeudInt]->Print(lang);
         cout << ")";
      }
      else
      {
         cout << "!";
         noeud[idNoeudInt]->Print(lang);
      }
   }
};

Noeud Neg(Noeud n)
{
   int id = nbNoeuds++;
   noeud[id] = new nNeg(id, n.id);
   return *noeud[id];
}

Noeud operator ! (Noeud n)
{
   return Neg(n);
}

// Op

class Op : public Noeud
{
   public:

   virtual void PrintSigne(int lang) {}

   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      if((int)(fils[this->id].size()) > 0)
         noeud[fils[this->id][0]]->Print(lang);
      for(int iFils = 1; iFils < (int)(fils[this->id].size()); iFils++)
      {
         PrintSigne(lang);
         noeud[fils[this->id][iFils]]->Print(lang);
      }
   }
};
 
// Aff

class nAff : public Op
{
   public :
   nAff(int _id) { this->id = _id; }
   virtual void PrintSigne(int lang) 
   {
      if(lang == cC || lang == cCPP || lang == cJAVA || 
         lang == cJVS || lang == cPYTHON) 
         cout << " = "; 
      if(lang == cPSEUDO)
         cout << " <- ";
   }   
};


template<typename ... Args>
Noeud Aff(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nAff(id);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

// EuclDiv

class nEuclDiv : public Op
{
   public :
   nEuclDiv(int _id) { this->id = _id; }
   virtual void PrintSigne(int lang) 
   {
      if(lang == cC || lang == cCPP || lang == cJAVA || lang == cJVS) 
         cout << " / "; 
      if(lang == cPYTHON)
         cout << " // ";
   }   
};


template<typename ... Args>
Noeud EuclDiv(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nEuclDiv(id);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

// Modulo

class nModulo : public Op
{
   public :
   nModulo(int _id) { this->id = _id; }
   virtual void PrintSigne(int lang) 
   {
      cout << " % ";
   }   
};


template<typename ... Args>
Noeud Modulo(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nModulo(id);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

// And

class nAnd : public Op
{
   public :
   nAnd(int _id) { this->id = _id; }
   virtual void PrintSigne(int lang) 
   {
      if(lang == cCPP || lang == cC || lang == cJAVA || lang == cJVS) 
         cout << " && "; 
      if(lang == cPSEUDO) 
         cout << " et ";
      if(lang == cPYTHON)
         cout << " and ";
   }   
};


template<typename ... Args>
Noeud And(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nAnd(id);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

Noeud operator && (Noeud n1, Noeud n2) { return And(n1, n2); }

// Or

class nOr : public Op
{
   public :
   nOr(int _id) { this->id = _id; }
   virtual void PrintSigne(int lang) 
   {
      if(lang == cCPP || lang == cC || lang == cJAVA || lang == cJVS) 
         cout << " || "; 
      if(lang == cPSEUDO) 
         cout << " ou ";
      if(lang == cPYTHON)
         cout << " or ";
   }
};


template<typename ... Args>
Noeud Or(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nOr(id);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

Noeud operator || (Noeud n1, Noeud n2) { return Or(n1, n2); }

// Plus

class nPlus : public Op
{
   public :
   nPlus(int _id) { this->id = _id; }
   virtual void PrintSigne(int lang) { cout << " + "; }   
};


template<typename ... Args>
Noeud Plus(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nPlus(id);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

Noeud operator + (Noeud n1, Noeud n2) { return Plus(n1, n2); }
Noeud operator + (int n1, Noeud n2) { return Plus(n1, n2); }
Noeud operator + (Noeud n1, int n2) { return Plus(n1, n2); }

// Soustract

class nSoustract : public Op
{
   public :
   nSoustract(int _id) { this->id = _id; }
   virtual void PrintSigne(int lang) { cout << " - "; }   
};


template<typename ... Args>
Noeud Soustract(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nSoustract(id);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

Noeud operator - (Noeud n1, Noeud n2) { return Soustract(n1, n2); }
Noeud operator - (int n1, Noeud n2)   { return Soustract(n1, n2); }
Noeud operator - (Noeud n1, int n2)   { return Soustract(n1, n2); }

// Prod

class nProd : public Op
{
   public :
   nProd(int _id) { this->id = _id; }
   virtual void PrintSigne(int lang) { cout << " * "; }   
};


template<typename ... Args>
Noeud Prod(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nProd(id);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

Noeud operator * (Noeud n1, Noeud n2) { return Prod(n1, n2); }
Noeud operator * (int n1, Noeud n2) { return Prod(n1, n2); }
Noeud operator * (Noeud n1, int n2) { return Prod(n1, n2); }

// Egal

const int cEQUAL = 120832;

class nEgal : public Op
{
   public :
   nEgal(int _id) { this->id = _id; this->type = cEQUAL; }
   virtual void PrintSigne(int lang) { cout << " == "; }   
};


template<typename ... Args>
Noeud Egal(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nEgal(id);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

Noeud operator == (Noeud n1, Noeud n2) { return Egal(n1, n2); }
Noeud operator == (int n1, Noeud n2) { return Egal(n1, n2); }
Noeud operator == (Noeud n1, int n2) { return Egal(n1, n2); }


// Diff

const int cDIFF = 928173;

class nDiff : public Op
{
   public :
   nDiff(int _id) { this->id = _id; this->type = cDIFF; }
   virtual void PrintSigne(int lang) { cout << " != "; }   
};


template<typename ... Args>
Noeud Diff(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nDiff(id);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

Noeud operator != (Noeud n1, Noeud n2) { return Diff(n1, n2); }
Noeud operator != (int n1, Noeud n2) { return Diff(n1, n2); }
Noeud operator != (Noeud n1, int n2) { return Diff(n1, n2); }

// Inf Strict

const int cINF_STRICT = 8270;

class nInfStrict : public Op
{
   public :
   nInfStrict(int _id) { this->id = _id; this->type = cINF_STRICT; }
   virtual void PrintSigne(int lang) { cout << " < "; }   
};


template<typename ... Args>
Noeud InfStrict(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nInfStrict(id);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

Noeud operator < (Noeud n1, Noeud n2) { return InfStrict(n1, n2); }
Noeud operator < (int n1, Noeud n2) { return InfStrict(n1, n2); }
Noeud operator < (Noeud n1, int n2) { return InfStrict(n1, n2); }

// Inf

const int cINF = 8271;

class nInf : public Op
{
   public :
   nInf(int _id) { this->id = _id;  this->type = cINF; }
   virtual void PrintSigne(int lang) { cout << " <= "; }   
};

template<typename ... Args>
Noeud Inf(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nInf(id);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

Noeud operator <= (Noeud n1, Noeud n2) { return Inf(n1, n2); }
Noeud operator <= (int n1, Noeud n2) { return Inf(n1, n2); }
Noeud operator <= (Noeud n1, int n2) { return Inf(n1, n2); }



// Sup Strict

const int cSUP_STRICT = 8272;

class nSupStrict : public Op
{
   public :
   nSupStrict(int _id) { this->id = _id; this->type = cSUP_STRICT; }
   virtual void PrintSigne(int lang) { cout << " > "; }   
};


template<typename ... Args>
Noeud SupStrict(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nSupStrict(id);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

Noeud operator > (Noeud n1, Noeud n2) { return SupStrict(n1, n2); }
Noeud operator > (int n1, Noeud n2) { return SupStrict(n1, n2); }
Noeud operator > (Noeud n1, int n2) { return SupStrict(n1, n2); }

// Sup

const int cSUP = 8273;

class nSup : public Op
{
   public :
   nSup(int _id) { this->id = _id;  this->type = cSUP; }
   virtual void PrintSigne(int lang) { cout << " >= "; }   
};

template<typename ... Args>
Noeud Sup(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nSup(id);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

Noeud operator >= (Noeud n1, Noeud n2) { return Sup(n1, n2); }
Noeud operator >= (int n1, Noeud n2) { return Sup(n1, n2); }
Noeud operator >= (Noeud n1, int n2) { return Sup(n1, n2); }

#endif

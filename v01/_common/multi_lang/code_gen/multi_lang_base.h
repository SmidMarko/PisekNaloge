#ifndef MULTI_LANG_BASE_H
#define MULTI_LANG_BASE_H

#include <iostream>
#include <algorithm>
#include <vector>
#include "string.h"
using namespace std;

const int MAX_NB_NOEUDS = 1000*1000;
const int MAX_NB_MODES  = 1000*1000;

// type
const int cVOID = 0, cINT = 1, cFLOAT = 2,
          cBOOL = 3, cSTRING = 4, cCHAR = 5, cPRINT_STRING = 42;

// lib
const int lVOID = 0, lREAD = 1, lPRINT = 2,
lMATH = 3, lSTRING = 4, lBOOL = 5;


// lang
const int cC = 0, cCPP = 1, cJAVA = 2, cJVS = 3, 
          cPYTHON = 4, cPSEUDO = 5; 

string lib_name = "robot";

void espaces(int nb)
{
   for(int i = 0; i < nb; i++)
      cout << "   ";
}

void PrintType(int lang, int type)
{
   if(type == cVOID)
      cout << "void";
   if(type == cINT)
      cout << "int";
   if(type == cFLOAT)
      cout << "float";
   if(type == cBOOL)
   {
      if(lang == cJAVA || lang == cJVS)
         cout << "boolean"; 
      else
         cout << "bool";
   }
   if(type == cSTRING)
   {
      if(lang == cJAVA || lang == cJVS)
         cout << "String";
      else
         cout << "string";
   }
   if(type == cCHAR)
      cout << "char";
}

void PrintDefVal(int lang, int type)
{
   if(type == cVOID || type == cINT)
      cout << "0";
   if(type == cFLOAT)
      cout << "0.";
   if(type == cBOOL)
   {
      if(lang == cPYTHON)
         cout << "False";
      if(lang == cPSEUDO)
         cout << "Faux";
      if(lang == cCPP || lang == cC || lang == cJAVA || lang == cJVS)
         cout << "true";   
   }
   if(type == cCHAR)
      cout << "\'a\'";
}


class Noeud
{
   public :
   int id, type, lib;
   Noeud() {type = cVOID; lib = lVOID;}
   virtual void Print(int lang, int level = 0) 
   { cout << "Print non def" << endl;  }

   virtual void SetDefReadVariable(bool val);

   virtual bool MultiLines(int lang){ return false; }

   virtual void PrintSeparator(int lang)
   {
      if(lang == cC || lang == cCPP || lang == cJAVA || lang == cJVS)  
         cout << ";";
      cout << endl; 
   }

};

Noeud* noeud[MAX_NB_NOEUDS];
bool defRead[MAX_NB_NOEUDS];
vector<int> fils[MAX_NB_NOEUDS];
vector<int> autresFils[MAX_NB_NOEUDS];
int nbNoeuds;

void Noeud::SetDefReadVariable(bool val) 
{
   for(int idFils : fils[this->id])
      noeud[idFils]->SetDefReadVariable(val);
}


Noeud cVoid(Noeud n) { noeud[n.id]->type = cVOID; return *noeud[n.id]; }
Noeud cInt(Noeud n) { noeud[n.id]->type = cINT; return *noeud[n.id]; }
Noeud cFloat(Noeud n) { noeud[n.id]->type = cFLOAT; return *noeud[n.id]; }
Noeud cBool(Noeud n) { noeud[n.id]->type = cBOOL; return *noeud[n.id]; }

// Blank
class nBlank : public Noeud
{
   public : 
   nBlank(int _id) { this->id = _id; }
   virtual void PrintSeparator(int lang) {  }
   virtual void Print(int lang, int level = 0) { cout << endl;  }
};

Noeud Blank()
{
   int id = nbNoeuds++;
   noeud[id] = new nBlank(id);
   return *noeud[id];
}

// Comment

class nComment : public Noeud
{
   public : 
   string str;
   nComment(int _id, string _str) { this->id = _id; str = _str; }
   virtual void PrintSeparator(int lang) {  }
   virtual void Print(int lang, int level = 0) 
   {
      espaces(level);
      if(lang == cC || lang == cJAVA ||lang == cCPP || lang == cJVS)
         cout << "// ";
      if(lang == cPYTHON)
         cout << "# ";
      cout << str << endl;
   }
};

Noeud Comment(string str)
{
   int id = nbNoeuds++;
   noeud[id] = new nComment(id,str);
   return *noeud[id];
}

// ContainsLib

bool containsLib(int idNoeud, int lib)
{
   if(noeud[idNoeud]->lib == lib)
      return true;

   for(int idFils : fils[idNoeud])
      if(containsLib(idFils,lib))
         return true;

   for(int idFils : autresFils[idNoeud])
      if(containsLib(idFils,lib))
         return true;

   return false;
}

// Mot

class nMot : public Noeud
{
   public :
   string mot;
   nMot(int _id, string _mot) { this->id = _id; mot = _mot; }
   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      cout << mot;
   }
};

Noeud Mot(string mot)
{
   int id = nbNoeuds++;
   noeud[id] = new nMot(id, mot);
   return *noeud[id];
}

// Entier

class nEntier : public Noeud
{
   public :
   int val;
   nEntier(int _id, int _val) { this->id = _id; val = _val; }
   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      cout << val;
   }
};

Noeud Entier(int val)
{
   int id = nbNoeuds++;
   noeud[id] = new nEntier(id, val);
   return *noeud[id];
}

// Bool

class nBool : public Noeud
{
   public :
   bool val;
   nBool(int _id, bool _val) 
   {
      this->lib = lBOOL; 
      this->id = _id; 
      val = _val; 
   }
   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      if(val)
      {
         if(lang == cC || lang == cCPP || lang == cJAVA || lang == cJVS) 
            cout << "true";
         if(lang == cPYTHON)
            cout << "True";
         if(lang == cPSEUDO)
            cout << "Vrai";
      }  
      else
      {
         if(lang == cC || lang == cCPP || lang == cJAVA || lang == cJVS) 
            cout << "false";
         if(lang == cPYTHON)
            cout << "False";
         if(lang == cPSEUDO)
            cout << "Faux";
      }
   }
};

Noeud Bool(bool val)
{
   int id = nbNoeuds++;
   noeud[id] = new nBool(id, val);
   return *noeud[id];
}

Noeud True()  { return Bool(true);  }
Noeud False() { return Bool(false); } 

// Bloc

class nBloc 
{
   public:
   int id;

   nBloc(int _id) : id(_id) {}

   void ajoute() {}

   template<typename ...Args> 
   void ajoute(Noeud n, Args... args)
   {
      fils[id].push_back(n.id);
      ajoute(args...);
   }

   template<typename ...Args> 
   void ajoute(int val, Args... args)
   {
      Noeud v = Entier(val);
      fils[id].push_back(v.id);
      ajoute(args...);
   }

   template<typename ...Args> 
   void ajoute(string mot, Args... args)
   {
      Noeud m = Mot(mot);
      fils[id].push_back(m.id);
      ajoute(args...);
   }

   template<typename ...Args>
   Noeud operator () (Args... args)
   {
      ajoute(args...);
      return *noeud[id];
   }

};


nBloc Bloc(int id)
{
   return nBloc(id);
}

// Text

class nText : public Noeud
{
   public :
   nText(int _id) {this->id = _id; }

   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      for(int iFils = 0; iFils < (int)(fils[this->id].size()); iFils++)
         noeud[fils[this->id][iFils]]->Print(lang);
   }   
};

template<typename ...Args>
Noeud Text(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nText(id);
   nBloc b = Bloc(id);
   b(args...);
   return *noeud[id];
}

// String

Noeud String(string str)
{
   return Text("\"",str,"\"");
}

// Modes

int nbModes;
bool actif[MAX_NB_MODES];

class Mode
{
   public:

   int id;

   Mode();

   virtual bool isSatisfied() 
   { 
      return actif[id]; 
   }
};

Mode* _mode[MAX_NB_MODES];

Mode::Mode()
{
   id = nbModes++;
   _mode[id] = this;
}

// NegMode

class nNegMode : public Mode
{
   int idArg;

   public :

   nNegMode(int _id, int _idArg)
   {
      this->id = _id;
      idArg = _idArg;
   }

   virtual bool isSatisfied()
   {
      return !(_mode[idArg]->isSatisfied());
   }
};

Mode NegMode(Mode m)
{
   int id = nbModes++;
   _mode[id] = new nNegMode(id, m.id);
   return *_mode[id];   
}

Mode operator ! (Mode m) { return NegMode(m); }

// AndMode

class nAndMode : public Mode
{
   int idArg1, idArg2;

   public :

   nAndMode(int _id, int _idArg1, int _idArg2)
   {
      this->id = _id;
      idArg1 = _idArg1;
      idArg2 = _idArg2;
   }

   virtual bool isSatisfied()
   {
      return _mode[idArg1]->isSatisfied() && _mode[idArg2]->isSatisfied();
   }
};

Mode AndMode(Mode m1, Mode m2)
{
   int id = nbModes++;
   _mode[id] = new nAndMode(id, m1.id, m2.id);
   return *_mode[id];   
}

Mode operator && (Mode m1, Mode m2) { return AndMode(m1,m2);  }

// OrMode

class nOrMode : public Mode
{
   int idArg1, idArg2;

   public :

   nOrMode(int _id, int _idArg1, int _idArg2)
   {
      this->id = _id;
      idArg1 = _idArg1;
      idArg2 = _idArg2;
   }

   virtual bool isSatisfied()
   {
      return _mode[idArg1]->isSatisfied() || _mode[idArg2]->isSatisfied();
   }
};

Mode OrMode(Mode m1, Mode m2)
{
   int id = nbModes++;
   _mode[id] = new nOrMode(id, m1.id, m2.id);
   return *_mode[id];   
}

Mode operator || (Mode m1, Mode m2) { return OrMode(m1,m2); }

// Mode de langage

Mode C, CPP, JAVA, JVS, PYTHON, PSEUDO, USE_READ, LIB, NO_SCANNER;

// Activation et Desactivation des modes

void activeModes() {}

template<typename ...Args>
void activeModes(Mode m, Args... args) 
{ 
   actif[m.id] = true; 
   activeModes(args...);
}

void desactiveModes() {}

template<typename ...Args>
void desactiveModes(Mode m, Args... args) 
{ 
   actif[m.id] = false; 
   desactiveModes(args...);
}

// Print Code

template<typename ...Args>
void _Print(int lang, Noeud n, Args... args) 
{ 
   activeModes(args...);
   noeud[n.id]->SetDefReadVariable(true);
   noeud[n.id]->Print(lang); 
   noeud[n.id]->SetDefReadVariable(false);
   desactiveModes(args...);
}

template<typename ...Args>
void PrintC(Noeud n, Args ...args)
{
   actif[C.id] = true;
   _Print(cC, n, args...);
   actif[C.id] = false;
}

template<typename ...Args>
void PrintCPP(Noeud n, Args... args)
{
   actif[CPP.id] = true;
   _Print(cCPP, n, args...);
   actif[CPP.id] = false;
}

template<typename ...Args>
void PrintJAVA(Noeud n, Args ...args)
{
   actif[JAVA.id] = true;
   _Print(cJAVA, n, args...);
   actif[JAVA.id] = false;
}

template<typename ...Args>
void PrintJVS(Noeud n, Args ...args)
{
   actif[JVS.id] = true;
   _Print(cJVS, n, args...);
   actif[JVS.id] = false;
}

template<typename ...Args>
void PrintPYTHON(Noeud n, Args ...args)
{
   actif[PYTHON.id] = true;
   _Print(cPYTHON, n, args...);
   actif[PYTHON.id] = false;
}

template<typename ...Args>
void PrintPSEUDO(Noeud n, Args ...args)
{
   actif[PSEUDO.id] = true;
   _Print(cPSEUDO, n, args...);
   actif[PSEUDO.id] = false;
}


template<typename ...Args>
void PrintAll(Noeud n, Args... args)
{
   cout << endl << "C++:" << endl << endl;
   PrintCPP(n, args...);
   cout << endl << "C:" << endl << endl;
   PrintC(n, args...);
   cout << endl << "Python:" << endl << endl;
   PrintPYTHON(n, args...);
   cout << endl << "Java:" << endl << endl;
   PrintJAVA(n, args...);
   cout << endl << "JVS:" << endl << endl;
   PrintJVS(n, args...);
}


  // Ecr dans Fichier

template<typename ...Args>
void _PrintFile(int lang, Noeud n, const char* nomFichier, Args ...args) 
{ 
   freopen(nomFichier, "w", stdout);
   _Print(lang,n, args...);  
   fclose(stdout);
}

template<typename ...Args>
void PrintFileC(Noeud n, const char* nomFichier, Args ...args)
{
   actif[C.id] = true;
   _PrintFile(cC, n, nomFichier, args...);
   actif[C.id] = false;
}

template<typename ...Args>
void PrintFileCPP(Noeud n, const char* nomFichier, Args ...args)
{
   actif[CPP.id] = true;
   _PrintFile(cCPP, n, nomFichier, args...);
   actif[CPP.id] = false;
}

template<typename ...Args>
void PrintFileJAVA(Noeud n, const char* nomFichier, Args ...args)
{
   actif[JAVA.id] = true;
   _PrintFile(cJAVA, n, nomFichier, args...);
   actif[JAVA.id] = false;
}

template<typename ...Args>
void PrintFileJVS(Noeud n, const char* nomFichier, Args ...args)
{
   actif[JVS.id] = true;
   _PrintFile(cJVS, n, nomFichier, args...);
   actif[JVS.id] = false;
}

template<typename ...Args>
void PrintFilePYTHON(Noeud n, const char* nomFichier, Args ...args)
{
   actif[PYTHON.id] = true;
   _PrintFile(cPYTHON, n, nomFichier, args...);
   actif[PYTHON.id] = false;
}

template<typename ...Args>
void PrintFilePSEUDO(Noeud n, const char* nomFichier, Args ...args)
{
   actif[PSEUDO.id] = true;
   _PrintFile(cPSEUDO, n, nomFichier, args...);
   actif[PSEUDO.id] = false;
}

template<typename ...Args>
void PrintFileAll(Noeud n, string nomFichier, Args ...args)
{
   PrintFileCPP(n, (nomFichier + ".cpp").data(), args...);
   PrintFileC(n, (nomFichier +  ".c").data(), args...);
   PrintFilePYTHON(n, (nomFichier +  ".py").data(), args...);
   PrintFileJAVA(n, (nomFichier +  ".java").data(), args...);
   PrintFileJVS(n, (nomFichier +  ".jvs").data(), args...);
}


#endif

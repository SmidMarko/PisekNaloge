#ifndef MULTI_LANG_CHAR_H
#define MULTI_LANG_CHAR_H

#include "multi_lang_base.h"

// Char

class nChar : public Noeud
{
   public :
   char car;
   nChar(int _id, char _car) 
   { 
     this->id = _id; 
     this->type = cCHAR;
     car = _car;
   }
  
   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      if(lang == cPYTHON)
         cout << "\"" << car << "\"";
      else
         cout << "\'" << car << "\'";
   }
};

Noeud Char(char car)
{
   int id = nbNoeuds++;
   noeud[id] = new nChar(id, car);
   return *noeud[id];
}

// IntToChar

class nIntToChar : public Noeud
{
   public :
   int idNoeudInt;
   nIntToChar(int _id, int _idNoeudInt) 
   { this->id = _id; 
     this->type = cCHAR;
     idNoeudInt = _idNoeudInt; autresFils[this->id].push_back(idNoeudInt);} 
  
   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      if(lang == cPYTHON)
         cout << "chr(";
      else
         cout << "(char)(";
      noeud[idNoeudInt]->Print(lang);
      cout << ")";
   }
};


Noeud IntToChar(Noeud n)
{
   int id = nbNoeuds++;
   noeud[id] = new nIntToChar(id, n.id);
   return *noeud[id];
}

Noeud IntToChar(int a)
{
   int id = nbNoeuds++;
   noeud[id] = new nIntToChar(id, Entier(a).id);
   return *noeud[id];
}


// CharToInt

class nCharToInt : public Noeud
{
   public :
   int idNoeudInt;
   nCharToInt(int _id, int _idNoeudInt) 
   { this->id = _id; 
     this->type = cINT;
     idNoeudInt = _idNoeudInt; autresFils[this->id].push_back(idNoeudInt);} 
  
   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      if(lang == cPYTHON)
         cout << "ord(";
      else
         cout << "(int)(";
      noeud[idNoeudInt]->Print(lang);
      cout << ")";
   }
};

Noeud CharToInt(Noeud n)
{
   int id = nbNoeuds++;
   noeud[id] = new nCharToInt(id, n.id);
   return *noeud[id];
}

Noeud CharToInt(char c)
{
   int id = nbNoeuds++;
   noeud[id] = new nCharToInt(id, Char(c).id);
   return *noeud[id];
}

#endif

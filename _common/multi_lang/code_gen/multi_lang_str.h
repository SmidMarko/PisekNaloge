#ifndef MULTI_LANG_STR_H
#define MULTI_LANG_STR_H

#include "multi_lang_base.h"
#include "multi_lang_io.h"
#include "multi_lang_op.h"

class nStringDecl : public Noeud
{
   int taille;
   bool hasValIni;
   int idVar;   
   int idValIni;

   public : 

   nStringDecl(int _id, int _idVar, bool _hasValIni, int _taille, int _idValIni)
   {
      this->lib = lSTRING;
      this->id = _id;
      idVar = _idVar;
      hasValIni = _hasValIni;
      taille = _taille;
      idValIni = _idValIni;
      autresFils[this->id].push_back(idVar);
      if(hasValIni)
         autresFils[this->id].push_back(idValIni);
   }

   virtual void PrintSeparator(int lang)
   {
      if(!defRead[idVar] || lang == cCPP || lang == cC)
      {
         if(lang == cC)
            cout << ";";
         if(lang == cCPP || lang == cJAVA || lang == cJVS)
            cout << ";";
         cout << endl;
      }
   }

   virtual void Print(int lang, int level = 0)
   {    
      if(defRead[idVar] && (lang == cJAVA || 
                               lang == cJVS || lang == cPYTHON))
         return;      

      espaces(level);

      if(hasValIni)
      {
         if(lang == cPYTHON)
            noeud[idVar]->Print(lang);
         
         if(lang == cC)
         {
            cout << "char ";
            noeud[idVar]->Print(lang);
            cout << "[]";
         }
         
         if(lang == cCPP || lang == cJAVA || lang == cJVS)
         {
            if(lang == cCPP)
               cout << "string ";
            else
               cout << "String ";
            noeud[idVar]->Print(lang);
         }
         
         cout << " = ";
         noeud[idValIni]->Print(lang);
      }
      else
      {
         if(lang == cPYTHON)
         {
            noeud[idVar]->Print(lang);
            cout << " = \"\"";
         }
         if(lang == cC)
         {
            cout  << "char ";
            noeud[idVar]->Print(lang);
            cout << "[";
            cout << taille;
            cout << "]";
         }

         if(lang == cJAVA || lang == cJVS)
         {
            cout << "String ";
            noeud[idVar]->Print(lang);
         }
        
         if(lang == cCPP)
         {
            cout << "string ";
            noeud[idVar]->Print(lang);
         }
      }
   }
};

Noeud StringDecl(Noeud str, int taille)
{
   int id = nbNoeuds++;
   noeud[id] = new nStringDecl(id,str.id,false,taille,0);
   return *noeud[id];
}

Noeud StringDecl(Noeud str, Noeud valIni)
{
   int id = nbNoeuds++;
   noeud[id] = new nStringDecl(id,str.id,true,0,valIni.id);
   return *noeud[id];
}

Noeud StringDecl(Noeud str, string valIni)
{
   int id = nbNoeuds++;
   noeud[id] = new nStringDecl(id,str.id,true,0,
                               Text("\"",valIni, "\"").id);
   return *noeud[id];
}

// GetLine

class nGetLine : public Noeud
{
   int idNoeud;
   bool def;
   public :
   nGetLine(int _id, int _idNoeud, bool _def = false)
   {
      this->lib = lSTRING;
      this->id = _id;
      idNoeud = _idNoeud;
      def = _def;
      autresFils[this->id].push_back(idNoeud);
   }

   virtual void SetDefReadVariable(bool val)
   {
      if(def)
         defRead[idNoeud] = val;
   }

   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      if(lang == cPYTHON)
      {
         noeud[idNoeud]->Print(lang);
         cout << " = input()";
      }
 
      if(lang == cCPP)
      {
         cout << "getline(cin, ";
         noeud[idNoeud]->Print(lang);
         cout << ")";
      }

      if(lang == cC)
      {
         cout << "scanf(\"%[^\\n]\\n\", ";
         noeud[idNoeud]->Print(lang);
         cout << ")";
      }

      if(lang == cJAVA || lang == cJVS)
      {
         if(def)
            cout << "String ";
         noeud[idNoeud]->Print(lang);

         if(lang == cJAVA)
            cout << " = input.nextLine()";
         else
            cout << " = readString()";
      }
   }
};

Noeud GetLine(Noeud n)
{
   int id = nbNoeuds++;
   noeud[id] = new nGetLine(id, n.id, false);
   return *noeud[id];
}

Noeud DefGetLine(Noeud n)
{
   int id = nbNoeuds++;
   noeud[id] = new nGetLine(id, n.id, true);
   return *noeud[id];
}

// Ignore

class nIgnore : public Noeud
{
   public:

   nIgnore(int _id) { this->id = _id; }

   virtual void PrintSeparator(int lang)
   {
      if(lang == cCPP)
         cout << ";" << endl;
   }

   virtual void Print(int lang, int level = 0)
   {
      if(lang == cCPP)
      {
         espaces(level);
         cout << "cin.ignore()";
      }
   }
};

Noeud Ignore()
{
   int id = nbNoeuds++;
   noeud[id] = new nIgnore(id);
   return *noeud[id];
}

// String Comp

class nStringComp : public Noeud
{
   int idGauche, idDroite, type;
   public :
   nStringComp(int _id, int _idGauche, int _idDroite, int _type)
   {
      this->lib = lSTRING;
      this->id = _id;
      idGauche = _idGauche;
      idDroite = _idDroite;
      type = _type;
      autresFils[this->id].push_back(idGauche);
      autresFils[this->id].push_back(idDroite);
   }

   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      if(lang == cCPP || lang == cPYTHON)
      {
         noeud[idGauche]->Print(lang);
         
         if(type == cINF_STRICT)
            cout << " < ";
         if(type == cSUP_STRICT)
            cout << " > ";
         if(type == cINF)
            cout << " <= ";
         if(type == cSUP)
            cout << " >= ";
         if(type == cEQUAL)
            cout << " == ";
         if(type == cDIFF)
            cout << " != ";         

         noeud[idDroite]->Print(lang);
      }

      if(lang == cC || lang == cJAVA || lang == cJVS)
      {
         if(lang == cC)
         {
            cout << "strcmp(";
            noeud[idGauche]->Print(lang);
            cout << ", ";
            noeud[idDroite]->Print(lang);
            cout << ")";
         }
         else
         {
            noeud[idGauche]->Print(lang);
            cout << ".compareTo(";
            noeud[idDroite]->Print(lang);
            cout << ")";
         }

         if(type == cINF_STRICT)
            cout << " < ";
         if(type == cSUP_STRICT)
            cout << " > ";
         if(type == cINF)
            cout << " <= ";
         if(type == cSUP)
            cout << " >= ";
         if(type == cEQUAL)
            cout << " == ";
         if(type == cDIFF)
            cout << " != "; 

         cout << "0";
      }

   } 
};

Noeud StringComp(Noeud comp)
{
   int id = nbNoeuds++;
   int idGauche = 0, idDroite = 0;
   
   if((int)(fils[comp.id].size()) >= 2)
   {
      idGauche = fils[comp.id][0];
      idDroite = fils[comp.id][1];
   }

   noeud[id] = new nStringComp(id, idGauche, idDroite, comp.type);
   return *noeud[id];
}

// StringLength

class nStringLength : public Noeud
{
   public :
   int idNoeudInt;
   nStringLength(int _id, int _idNoeudInt) 
   { this->lib = lSTRING;
     this->id = _id; idNoeudInt = _idNoeudInt; 
     autresFils[this->id].push_back(idNoeudInt);} 
  
   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      if(lang == cPYTHON)
      {
         cout << "len(";
         noeud[idNoeudInt]->Print(lang);
         cout << ")";
      }
      if(lang == cJAVA || lang == cJVS) 
      {
         noeud[idNoeudInt]->Print(lang);
         cout << ".length()";
      }
      if(lang == cCPP)
      {
         cout << "(int)(";
         noeud[idNoeudInt]->Print(lang);
         cout<< ".length())";
      }
      if(lang == cC)
      {
         cout << "strlen(";
         noeud[idNoeudInt]->Print(lang);
         cout << ")";        
      }
   }
};

Noeud StringLength(Noeud n)
{
   int id = nbNoeuds++;
   noeud[id] = new nStringLength(id, n.id);
   return *noeud[id];
}

#endif

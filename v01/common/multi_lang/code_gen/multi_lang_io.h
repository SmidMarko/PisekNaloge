#ifndef MULTI_LANG_IO_H
#define MULTI_LANG_IO_H

#include "multi_lang_base.h"
#include "multi_lang_cf.h"


// Read

class nRead : public Noeud
{
   bool lineMode, def;
   public :
   nRead(int _id, int _lineMode = false, bool _def = false) 
   {
      this->id = _id;
      this->type = cVOID; 
      this->lib = lREAD; 
      lineMode = _lineMode;
      def = _def;
   }

   virtual bool MultiLines(int lang)
   {
      if(lang == cC || lang == cCPP || lang == cPYTHON)  
         return false;
      if(lang == cJAVA || lang == cJVS)
         return (int)(fils[this->id].size()) > 1;
      return false; 
   }

   virtual void PrintSeparator(int lang) {}

   virtual void SetDefReadVariable(bool val)
   {
      if(def)
         for(int idFils : fils[this->id])
            defRead[idFils] = val;   
   }

   virtual void Print(int lang, int level = 0)
   {
      if(lang == cCPP)
      {
         espaces(level);
         cout << "cin";
         for(int idFils : fils[this->id])
         {
            cout << " >> ";
            noeud[idFils]->Print(lang);
         }
         cout << ";" << endl;
      }
      if(lang == cC)
      {
         espaces(level);
         cout << "scanf(\"";
         for(int idFils : fils[this->id])
            if(noeud[idFils]->type == cVOID || noeud[idFils]->type == cINT )
               cout << "%d";
            else if(noeud[idFils]->type == cFLOAT)
               cout << "%f";
            else if(noeud[idFils]->type == cCHAR)
               cout << "%c";
         cout << "\"";
         for(int idFils : fils[this->id])
         {
            cout << ", &";
            noeud[idFils]->Print(lang);
         }
         cout << ");" << endl;
      }
      if(lang == cPSEUDO)
      {
         for(int idFils : fils[this->id])
         {
            espaces(level);
            noeud[idFils]->Print(lang);
            cout << " <- Lire";
            if(noeud[idFils]->type == cVOID || noeud[idFils]->type == cINT )
               cout << "Entier()";
            if(noeud[idFils]->type == cFLOAT)
               cout << "Decimal()"; 
            cout << endl;
         }
      }
      if(lang == cPYTHON)
      {
         if(!lineMode)
         {
            for(int idFils : fils[this->id])
            {
               espaces(level);
               noeud[idFils]->Print(lang);
               cout << " = ";
               if(noeud[idFils]->type == cVOID)
                  cout << "int";
               else
                  PrintType(lang,noeud[idFils]->type); 
               cout << "(input())" << endl;
            }
         }
         else
         {
            espaces(level);
            bool prem = true;
            for(int idFils : fils[this->id])
            {
               if(!prem)
                  cout << ", ";
               prem = false;
               noeud[idFils]->Print(lang);
            }
            cout << " = map(";
            PrintType(lang, noeud[fils[this->id][0]]->type);
            cout << ", input().split())" << endl;
         }
      }
      if(lang == cJAVA)
      {
         for(int idFils : fils[this->id])
         {
            espaces(level);
            if(defRead[idFils])
            {
               PrintType(lang, noeud[idFils]->type);
               cout << " ";
            }
            noeud[idFils]->Print(lang);
            cout << " = input.nextInt();" << endl;
            // TODO : voir pour autres types
         } 
      }
      if(lang == cJVS)
      {
         for(int idFils : fils[this->id])
         {
            espaces(level);
            if(defRead[idFils])
            {
               PrintType(lang, noeud[idFils]->type);
               cout << " ";
            }
            noeud[idFils]->Print(lang);
            cout << " = readInt();" << endl;
            // TODO : voir pour autres types
         } 
      }
   }
};

template<typename ...Args>
Noeud Read(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nRead(id);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

template<typename ...Args>
Noeud DefRead(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nRead(id,false,true);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

template<typename ...Args>
Noeud ReadLine(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nRead(id,true);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

template<typename ...Args>
Noeud DefReadLine(Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nRead(id,true,true);
   nBloc B = Bloc(id);
   B(args...);
   return *noeud[id];
}

// PrintString

class nPrintString : public Noeud
{
   public :
   string str;
   nPrintString(int _id, string _str) 
   { 
     this->id = _id; 
     this->type = cPRINT_STRING;
     str = _str; 
   }
   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      cout << str;
   }
};

Noeud PrintString(string str)
{
   int id = nbNoeuds++;
   noeud[id] = new nPrintString(id, str);
   return *noeud[id];
}

//Print

class nPrint : public Noeud
{
   public :
   bool withEndl;
   nPrint(int _id, bool _withEndl ) 
   {
      this->lib = lPRINT;
      this->id = _id; 
      withEndl = _withEndl; 
   }

   void ajoute() {}

   template<typename ...Args> 
   void ajoute(Noeud n, Args... args)
   {
      fils[this->id].push_back(n.id);
      ajoute(args...);
   }

   template<typename ...Args> 
   void ajoute(int val, Args... args)
   {
      Noeud v = Entier(val);
      fils[this->id].push_back(v.id);
      ajoute(args...);
   }

   template<typename ...Args> 
   void ajoute(string str, Args... args)
   {
      Noeud s = PrintString(str);
      fils[this->id].push_back(s.id);
      ajoute(args...);
   }
   
   void creerFils() {}

   template<typename ...Args>
   void creerFils(Args... args)
   {
      ajoute(args...);
   }

   virtual void PrintSeparator(int lang) {}

   virtual bool MultiLines(int lang)
   {
      if(lang == cC || lang == cCPP || lang == cPYTHON)  
         return false;
      if(lang == cJAVA || lang == cJVS)
         return (int)(fils[this->id].size()) > 1;
      return false; 
   }

   virtual void Print(int lang, int level = 0)
   {
      if(lang == cCPP)
      {
         espaces(level);
         cout << "cout";
         for(int idFils : fils[this->id])
            if(noeud[idFils]->type == cPRINT_STRING)
            {
               cout << " << \"";
               noeud[idFils]->Print(lang);
               cout << "\"";
            }
            else
            {
               cout << " << ";
               noeud[idFils]->Print(lang);
            }

         if(withEndl)
            cout << " << endl";
         cout << ";" << endl;
      }
      if(lang == cC)
      {
         espaces(level);
         cout << "printf(\"";      
         for(int idFils : fils[this->id])
            if(noeud[idFils]->type == cVOID || noeud[idFils]->type == cINT )
               cout << "%d";
            else if(noeud[idFils]->type == cFLOAT)
               cout << "%f";
            else if(noeud[idFils]->type == cCHAR)
               cout << "%c";
            else if(noeud[idFils]->type == cSTRING)
               cout << "%s";
            else if(noeud[idFils]->type == cPRINT_STRING)
               noeud[idFils]->Print(lang);
         if(withEndl)
            cout << "\\n";
         cout << "\"";
         for(int idFils : fils[this->id])
            if(noeud[idFils]->type != cPRINT_STRING)
            {
               cout << ", ";
               noeud[idFils]->Print(lang);
            }
         cout <<");" << endl;
      }

      if(lang == cJAVA || lang == cJVS || lang == cPYTHON)
      {
         int size = (int)(fils[this->id].size());
         for(int iFils = 0; iFils < size - 1; iFils++)
         {
            int idFils = fils[this->id][iFils];
            espaces(level);
            if(lang == cJAVA)
               cout << "System.out.";
            cout << "print(";

            if(noeud[idFils]->type == cPRINT_STRING)
            {
               cout << "\"";
               noeud[idFils]->Print(lang);
               cout << "\"";
            }
            else
               noeud[idFils]->Print(lang);

            if(lang == cPYTHON)
               cout << ", end=\"\")" << endl;
            if(lang == cJAVA || lang == cJVS)
               cout << ");" << endl;
         }      

         int idFils = -1;
         if((int)(fils[this->id].size()) > 0)
            idFils = fils[this->id][size - 1];
         
         espaces(level);
         if(lang == cJAVA)
            cout << "System.out.";
         if(lang == cPYTHON)
            cout << "print(";
         else if(withEndl)
            cout << "println(";
         else
            cout << "print(";

         if(idFils != -1)
         {
            if(noeud[idFils]->type == cPRINT_STRING)
            {
               cout << "\"";
              noeud[idFils]->Print(lang);
               cout << "\"";
            }
            else
               noeud[idFils]->Print(lang);
         }
         else
            cout << "\"\"";

         if(lang == cJAVA || lang == cJVS)
            cout << ");" << endl;
         else if(withEndl)
            cout << ")" << endl;
         else
            cout << ", end=\"\")" << endl;
      }
   }
};

template<typename ...Args>
Noeud Print(Args... args)
{
   int id = nbNoeuds++;
   nPrint* p = new nPrint(id, false);
   noeud[id] = p;
   p->creerFils(args...);
   return *noeud[id]; 
}

template<typename ...Args>
Noeud PrintLine(Args... args)
{
   int id = nbNoeuds++;
   nPrint* p = new nPrint(id, true);
   noeud[id] = p;
   p->creerFils(args...);
   return *noeud[id]; 
}

#endif

#ifndef MULTI_LANG_DECL_H
#define MULTI_LANG_DECL_H

#include "multi_lang_base.h"
#include "multi_lang_io.h"

const int DEFAULT = -1;

class nDecl : public Noeud
{
   public:
   int idGauche, idDroit;    
   bool estConst;

   nDecl(int _id, int _idGauche, int _idDroit, bool _estConst)
   {
      this->id = _id;
      idGauche = _idGauche;
      idDroit  = _idDroit;
      estConst = _estConst;
      autresFils[this->id].push_back(idGauche);
      if(idDroit != DEFAULT)
         autresFils[this->id].push_back(idDroit);
   }

   virtual void PrintSeparator(int lang)
   {
      if(!defRead[idGauche] || lang == cCPP || lang == cC)
      {
         if(lang == cC && !estConst)
            cout << ";";
         if(lang == cCPP || lang == cJAVA || lang == cJVS)
            cout << ";";
         cout << endl;
      }
   }

   virtual void Print(int lang, int level = 0)
   {
     
      if(defRead[idGauche] && (lang == cJAVA || 
                               lang == cJVS || lang == cPYTHON))
         return;      

      espaces(level);

      if(level == 1 && lang == cJAVA)
         cout << "static ";  

      if(estConst)
      {
         if(lang == cC)
         {
            cout << "#define ";
            noeud[idGauche]->Print(lang);
            cout << " (";
            noeud[idDroit]->Print(lang);
            cout << ")";
         }
         if(lang == cCPP || lang == cJAVA || lang == cJVS)
         {
            if(lang == cCPP)
               cout << "const ";
            if(lang == cJAVA || lang == cJVS)
               cout << "final ";

            PrintType(lang, noeud[idGauche]->type);
            cout << " ";
            noeud[idGauche]->Print(lang);
            cout << " = ";
            noeud[idDroit]->Print(lang);
         }
         if(lang == cPYTHON)
         {
            noeud[idGauche]->Print(lang);
            cout << " = ";
            noeud[idDroit]->Print(lang);
         }
         
         return;
      }
   
      if(lang != cPYTHON)
      {  
         PrintType(lang, noeud[idGauche]->type);
         cout << " ";
      }
      noeud[idGauche]->Print(lang);
 
      if(idDroit != DEFAULT)
      {
         cout << " = ";
         noeud[idDroit]->Print(lang);
      }
      else if(lang == cPYTHON || lang == cJAVA || lang == cJVS)
      {
         cout << " = ";
         PrintDefVal(lang,noeud[idGauche]->type); 
      }

   }
};

template<typename ...Args>
Noeud ConstDecl(Noeud gauche, Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nDecl(id, gauche.id, Text(args...).id, true);
   return *noeud[id];
}

template<typename ...Args>
Noeud Decl(Noeud gauche)
{
   int id = nbNoeuds++;
   noeud[id] = new nDecl(id, gauche.id, DEFAULT, false);
   return *noeud[id];
}

template<typename ...Args>
Noeud Decl(Noeud gauche, Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nDecl(id, gauche.id, Text(args...).id, false);
   return *noeud[id];
}

// ArrayDecl

class nArrayDecl : public Noeud
{
   int idTab;
   public :
   nArrayDecl(int _id, int _idTab) 
   { 
      this->id = _id; 
      idTab = _idTab;
      autresFils[this->id].push_back(idTab);
   }

   virtual void PrintSeparator(int lang) 
   {
      if(lang != cPYTHON)
         cout << ";" << endl;
      else if(!defRead[idTab])
         cout << endl;   
   }

   virtual void Print(int lang, int level = 0)
   {
      espaces(level);    

      if(lang == cCPP || lang == cC)
      {
         PrintType(lang, noeud[idTab]->type);
         cout << " ";
         noeud[idTab]->Print(lang);

         for(int idFils : fils[this->id] )
         {
            cout << "[";
            noeud[idFils]->Print(lang); 
            cout << "]"; 
         }
      }

      if(lang == cJAVA || lang == cJVS)
      {
         if(level == 1 && lang == cJAVA)
            cout << "static ";  

         PrintType(lang, noeud[idTab]->type);
         cout << " ";
         for(int id : fils[this->id])
            cout << "[]";
         cout << " ";
         noeud[idTab]->Print(lang);
         cout << " = new ";
         PrintType(lang, noeud[idTab]->type);
      
         for(int idFils : fils[this->id] )
         {
            cout << "[";
            noeud[idFils]->Print(lang); 
            cout << "]"; 
         }
      }

      if(lang == cPYTHON && !defRead[idTab])
      {
         noeud[idTab]->Print(lang);
         cout << " = ";
         for(int i : fils[this->id])
            cout << "[";
         PrintDefVal(lang, noeud[idTab]->type);
         cout << "]*("; 
         int size = (int)(fils[this->id].size());
         noeud[fils[this->id][size-1]]->Print(lang);
         cout << ")";
         for(int iFils = size - 2; iFils >= 0; iFils--)
         {
            cout << " for _ in range(";
            noeud[fils[this->id][iFils]]->Print(lang);
            cout << ")]";
         }
      }      
   }
};

template<typename ...Args>
Noeud ArrayDecl(Noeud tab, Args... args)
{
   int id = nbNoeuds++;
   noeud[id] = new nArrayDecl(id, tab.id);  
   nBloc B = Bloc(id);
   B(args...); 
   return *noeud[id];
}

// ReadArrayLine

class nReadArrayLine : public Noeud
{
   public :
   int idTab;
   int idRange;
   bool def;

   nReadArrayLine(int _id, int _idTab, int _idRange, bool _def = false)
   {
      this->id = _id;
      this->lib = lREAD;
      idTab = _idTab;
      idRange = _idRange;
      def = _def;
      autresFils[this->id].push_back(idTab);
      autresFils[this->id].push_back(idRange);
   }

   virtual void SetDefReadVariable(bool val) 
   {
      if(def)
         defRead[idTab] = val;
   }

   virtual void PrintSeparator(int lang)
   {
      if(lang == cPYTHON)
         cout << endl;
   }

   virtual void Print(int lang, int level = 0)
   {
      if(lang == cPYTHON)
      {
         if(defRead[idTab])
         {
            espaces(level);
            noeud[idTab]->Print(lang);
            cout << " = ";
            cout << "list(map(";
            PrintType(lang, noeud[idTab]->type);
            cout << ", input().split()))";
         }
         else
         {
            string nomTabTemp = "tabTemp";
            espaces(level);
            cout << nomTabTemp << " = list(map(";
            PrintType(lang, noeud[idTab]->type);
            cout << ", input().split()))" << endl;

            int idIter;
            if((int)(fils[fils[idRange][0]].size()) < 2)
               idIter = fils[idRange][0];
            else
               idIter = fils[fils[idRange][0]][1];

            int idFor = ForRange(*noeud[idRange])
            (
               Aff(CrochetExpr(*noeud[idTab],*noeud[idIter]),
                   Text(nomTabTemp,"[", *noeud[idIter],"]"))
            ).id;
            nForRange* forRange = (nForRange*)(noeud[idFor]); 
            
            if(!forRange->debNul || !forRange->incr)
            {
               Var iTabTemp("iTabTemp");
               int idBoucle = Group
               (
                  Decl(iTabTemp,0),
                  ForRange(*noeud[idRange])
                  (
                     Aff(CrochetExpr(*noeud[idTab],*noeud[idIter]),
                      Text(nomTabTemp,"[", iTabTemp,"]")),
                     iTabTemp = iTabTemp + 1
                  )
               ).id;
               noeud[idBoucle]->Print(lang,level);    
            }
            else
               noeud[idFor]->Print(lang,level); 
         }
      }
      else
      {
         int idIter;
         if((int)(fils[fils[idRange][0]].size()) < 2)
            idIter = fils[idRange][0];
         else
            idIter = fils[fils[idRange][0]][1];

         int idTmp = ForRange(*noeud[idRange])
         (
            Read(CrochetExpr(*noeud[idTab],*noeud[idIter])) 
         ).id;
         noeud[idTmp]->Print(lang,level);
      }
   }
};

Noeud ReadArrayLine(Noeud tab, Noeud range)
{
   int id = nbNoeuds++;
   noeud[id] = new nReadArrayLine(id, tab.id, range.id, false);
   return *noeud[id];
}

Noeud DefReadArrayLine(Noeud tab, Noeud range)
{
   int id = nbNoeuds++;
   noeud[id] = new nReadArrayLine(id, tab.id, range.id, true);
   return *noeud[id];
}

template<typename A, typename B>
Noeud InclExcl(A n1, B n2)
{
   return Text(n1,n2);
}


#endif

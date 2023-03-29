#ifndef MULTI_LANG_EXPR_H
#define MULTI_LANG_EXPR_H

#include "multi_lang_op.h"

// Expr

class Expr : public Noeud
{
   public:
   Expr() {}
   Expr(int _id, int _type) {this->id = _id; this->type = _type;}

   template<typename ...Args>
   Expr operator [] (Args... args);
   
   template<typename ...Args>
   Expr operator () (Args... args);

   Noeud operator = (Noeud n2) { return Aff(*this,n2); }

   Noeud operator = (Expr n2) { return Aff(*this,n2); }

   Noeud operator = (int n2) { return Aff(*this,n2);  }
};


class nCrochetExpr : public Expr
{
   public:
   int idNoeudExt,idNoeudInt;

   nCrochetExpr() {}

   nCrochetExpr(int _id, int _idNoeudExt ,int _idNoeudInt) 
   {
      this->id = _id; 
      idNoeudExt = _idNoeudExt;
      idNoeudInt = _idNoeudInt;
      this->type = noeud[idNoeudExt]->type;
      autresFils[this->id].push_back(idNoeudInt);
      autresFils[this->id].push_back(idNoeudExt);
   }
 
   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      noeud[idNoeudExt]->Print(lang);
      cout << "[";
      noeud[idNoeudInt]->Print(lang);
      cout << "]";
   }

   Noeud operator = (Noeud n2) { return Aff(*this,n2); }

   Noeud operator = (nCrochetExpr n2) { return Aff(*this,n2); }

   Noeud operator = (int n2) { return Aff(*this,n2);  }
};


Expr CrochetExpr(Noeud nExt, Noeud nInt)
{
   int id = nbNoeuds++;
   Expr* pExpr = new nCrochetExpr(id, nExt.id, nInt.id);
   noeud[id] = pExpr; 
   return *pExpr;
}

template<typename ...Args>
Expr Expr::operator [] (Args... args)
{
   return CrochetExpr(*this, Text(args...));
}


class nParExpr : public Expr
{
   public:
   int idNoeudExt;

   nParExpr() {}

   nParExpr(int _id, int _idNoeudExt) 
   {
      this->id = _id; 
      idNoeudExt = _idNoeudExt;
      this->type = noeud[idNoeudExt]->type;
      autresFils[this->id].push_back(idNoeudExt);
   }
 
   virtual void Print(int lang, int level = 0)
   {
      espaces(level);
      noeud[idNoeudExt]->Print(lang);
      cout << "(";

      if((int)(fils[this->id].size()) > 0)                   
         noeud[fils[this->id][0]]->Print(lang);
      for(int iFils = 1; iFils < (int)(fils[this->id].size()); iFils++)
      {
         cout << ", ";
         noeud[fils[this->id][iFils]]->Print(lang);
      }

      cout << ")";
   }
};

template<typename ...Args>
Expr ParExpr(Noeud nExt, Args... args)
{
   int id = nbNoeuds++;
   Expr* pExpr = new nParExpr(id, nExt.id);
   noeud[id] = pExpr;
   nBloc B = Bloc(id);
   B(args...);
   return *pExpr;
}

template<typename ...Args>
Expr Expr::operator () (Args... args)
{
   return ParExpr(*this,args...);
}


// Var

class Var : public Expr
{
   string nom;
   public :
   Var(string _nom, int _type = cINT) 
   { 
      this->id = nbNoeuds++; 
      this->type = _type;
      nom = _nom; 
      noeud[this->id] = this;
      if(this->type == cBOOL)
         this->lib = lBOOL;
      if(this->type == cSTRING)
         this->lib = lSTRING;
   }

   virtual void Print(int lang, int level = 0) 
   {
      espaces(level);
      cout << nom;   
   }
 
   Noeud operator = (Noeud n2) { return Aff(*this,n2); }

   Noeud operator = (Var n2) { return Aff(*this,n2); }
   
   Noeud operator = (int n2) { return Aff(*this,n2);  }
};

#endif

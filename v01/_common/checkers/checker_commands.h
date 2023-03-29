#ifndef CHECKER_COMMANDS_H
#define CHECKER_COMMANDS_H
#include <map>

class CheckerCommands : public Checker {
public:
   CheckerCommands(int argc, char ** argv):Checker(argc, argv){};
protected:
   // The main checker function
   void check() {
      init();
      doCommands();
      endInputReached();
   }

   // Init function : read data before the first command
   virtual void init() = 0;

   // Read the commands and send them to 'asnalyseCommand'
   void doCommands();
   
   // Analyse the different commands
   virtual bool analyseCommand(string cmd, vector<string> args) = 0;

   // Test if there remains command and call "successButRemains" if any
   void success(string msg = "", int score = 100);

   // There was on other command after the success => failure ; Easy to override
   virtual void successButRemains(string msg, int score, string cmd);

   // The end of input has been reached but no success/failure yet
   virtual void endInputReached() = 0;



   // The list of runned commands
   vector< pair< string, vector<string> > > commands;

   //// Useful functions  ////
   // Check is the number of given argument is correct
   void checkNumArgs(vector<string> args, int nb, string cmd);

   // Returns a string representation of a command
   string getCmdRepr(pair< string, vector<string> > cmd);
   string getCmdRepr(string cmd, vector<string> args);
private:
   //// Reading stdin ////
   // Read a full command : if false, the error is in "error"
   // If "cmd" is equal to "" it means no more commands
   bool readCommand(string & cmd, vector<string> & args, string & error);

   // Returns true if the read is successful
   bool readLine(string & line);
};


void CheckerCommands::doCommands() {
   string line; 
   while (true) {
      string cmd, error;
      vector<string> args;
      if (!readCommand(cmd, args, error)) {
         internalError(error);
      }
      if (cmd == "") {
         break;
      }

      if (isDebug()) printf("%s\n", getCmdRepr(cmd, args).c_str());

      // Register the command
      commands.push_back(make_pair(cmd, args));
      if (commands.size() > 50 * 1000) 
         failure("Too many commands");

      // Call the command
      if (!analyseCommand(cmd, args)) {
         // Removes the last command (the one causing the problem)
         commands.pop_back();
         failure(format("unknown command '%s'", getCmdRepr(cmd, args).c_str()));
      }
   }
}

void CheckerCommands::success(string msg, int score) {
   string cmd, error;
   vector<string> args;
   // There was a line but un error occurred
   if (!readCommand(cmd, args, error)) {
      internalError(error);
   }
   // There was an other command, let the function decides what to do
   else if (cmd != "") {
      successButRemains(msg, score, getCmdRepr(cmd, args));
   }

   // If we reach this point, success !
   Checker::success(msg, score);
}

void CheckerCommands::successButRemains(string msg, int score, string cmd) {
   failure(format("Mission accomplie mais d'autres commandes sont exécutées ensuite."), 0);
}

//// Useful function  ////
void CheckerCommands::checkNumArgs(vector<string> args, int nb, string cmd) {
   if ((int)args.size() != nb) {
      internalError(format("invalid number of arguments for %s : %d instead of %d", cmd.c_str(), args.size(), nb));
   }
}

string CheckerCommands::getCmdRepr(pair< string, vector<string> > cmd) {
   return getCmdRepr(cmd.first, cmd.second);
}

string CheckerCommands::getCmdRepr(string cmd, vector<string> args) {
   string s;
   s = s + cmd + "(";
   for (int i = 0; i < (int)args.size(); i++) {
      s = s + (i==0?"":", ") + "\"" + args[i] +  "\"";
   }
   s = s + ")";
   return s;
}


//// Reading stdin ////

// Return false if error (details in the "error" variable)
// If "cmd" is equal to "" it means no more commands
bool CheckerCommands::readCommand(string & cmd, vector<string> & args, string & error) {
   cmd = "";
   args.clear();
   
   // Eliminates empty lines
   string line;
   while (true) {
      if (!readLine(line)) {
         // No more command to read, it's not an error
         return true;
      }
      if (line != "") {
         break;
      }
   }

   // "Text" line
   if (line != "CMD") {
      cmd = "printText";
      args.push_back(line);
      return true;
   }

   // It's a real command
   if (!readLine(line)) {
      error = "impossible to read the command name";
      return false;
   }
   cmd = line;

   if (!readLine(line)) {
      error = format("impossible to read the number of arguments for the command '%s'", cmd.c_str());
      return false;
   }
   const int nb = atoi(line.c_str());

   for (int i = 1; i <= nb; i++) {
      if (!readLine(line)) {
         error = format("Impossible to read the arg #%d for the command '%s'", i, cmd.c_str());
         return false;
      }
      args.push_back(line);
   }
   return true;
}

bool CheckerCommands::readLine(string & line) {
   char buf[3000 + 1];
   if (feof(fOut) || fgets(buf, 3000, fOut) == NULL)
      return false;
   buf[strlen(buf)-1] = '\0';
   line = string(buf);
   return true;
}



#endif
/*
TODO : 
- pour les commandes, gérer le fait qu'on veut afficher une sortie
=> overload de success et failure ?
- que faire en fin de truc, erreur ou pas s'il reste des choses ??
- que faire si erreur au milieu de la chose ? json ou pas, y compris pour internal...
==> déterminer un "json" output quelque part pour formatted les choses ??
- faire un truc pour lire la "prochaine commande"
- les fonctions de sorties peuvent être surchargées si on veut faire du json
*/

// donc 3 actions : good, good et reste qqc, failure
// avec le good qui peut rappeler le good et failure ?
// plus la liste des actions à ajouter à la fin + message si failure
// faire schéma pour voir comment tout ça doit s'ordonner !!
// + la fonction si on est arrivé à la toute fin, donc 4 cas spéciaux ??

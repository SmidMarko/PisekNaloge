#ifndef CHECKER_ROBOT_TBLR_H
#define CHECKER_ROBOT_TBLR_H

class CheckerRobotTBLR : public CheckerCommands {
public:
   CheckerRobotTBLR(int argc, char ** argv):CheckerCommands(argc, argv){};

   // After each command call 'checkerGrid' to check for success
   bool analyseCommand(string cmd, vector<string> args);

   virtual void checkerGrid(int x, int y, string cmd, vector<string> args) = 0;

   virtual void endInputReached();
};

bool CheckerRobotTBLR::analyseCommand(string cmd, vector<string> args) {
   if (cmd == "haut") {
      checkNumArgs(args, 0, cmd);
      checkerGrid(0, -1, cmd, args);
      return true;
   }
   if (cmd == "bas") {
      checkNumArgs(args, 0, cmd);
      checkerGrid(0, 1, cmd, args);
      return true;
   }
   if (cmd == "gauche") {
      checkNumArgs(args, 0, cmd);
      checkerGrid(-1, 0, cmd, args);
      return true;
   }
   if (cmd == "droite") {
      checkNumArgs(args, 0, cmd);
      checkerGrid(1, 0, cmd, args);
      return true;
   }
   if (cmd == "error") {//when ???
      char buf[3001];
      if (feof(fOut) || fgets(buf, 3000, fOut) == NULL)
         failure("Internal error : invalid command 'error', end of file");
      failure(buf);
      return true;
   }
   return false;
}

void CheckerRobotTBLR::endInputReached() {
   failure("Le robot n'a pas accompli sa mission");
}
#endif

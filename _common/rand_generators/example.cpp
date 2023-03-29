#include <iostream>
#include "rand_generators/rand.h"
 
int main() {
   int nbLines = 8, nbCols = 9;
 
   vector< vector<int> > maze = rand_maze(nbLines,nbCols);
 
   for(int iLine = 0; iLine < nbLines; iLine++) {
      for(int iCol = 0; iCol < nbCols; iCol++) {
         cout << maze[iLine][iCol] << " ";
      }
      cout << endl;
   }
    
   return 0;
}

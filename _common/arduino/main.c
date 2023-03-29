#include "libarduino.h"
#include "program.h"

int main () {
  // Read input
  int nbLoops;
  scanf("%d\n", &nbLoops);

  char readBuffer[100];
  while(!feof(stdin)) {
    if(fgets(readBuffer, 100, stdin) != NULL) {
      __inputBuffer.append(readBuffer);
    }
  }

  setup();
  for(int l = 0; l < nbLoops; l++) {
    loop();
  }
}

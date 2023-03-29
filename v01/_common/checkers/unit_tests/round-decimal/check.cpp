#include <stdio.h>
#include "../../common.h"

int main(int argc, char ** argv)
{
	DistRoundChecker* checker = new DistRoundChecker(argc, argv);
   //checker->setDebug(true);
	checker->check(1, 2);	
}

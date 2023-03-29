#include <stdio.h>
#include "../../common.h"

int main(int argc, char ** argv)
{
	DistFloorChecker* checker = new DistFloorChecker(argc, argv);
   //checker->setDebug(true);
	checker->check(1, 2);	
}

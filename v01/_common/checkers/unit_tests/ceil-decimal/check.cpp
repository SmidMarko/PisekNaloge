#include <stdio.h>
#include "../../common.h"

int main(int argc, char ** argv)
{
	DistCeilChecker* checker = new DistCeilChecker(argc, argv);
   //checker->setDebug(true);
	checker->check(1, 2);	
}

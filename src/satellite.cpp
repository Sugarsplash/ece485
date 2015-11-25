#include "mem.h"
#include <cstdio>


int main()
{
	M2 myM2;
	M3 myM3;

	int read;

	myM3.write(0,0x12);
	read = myM3.read(0);
	printf("TEST: %x, should be 0x12\n", read);


	myM2.write(0,0x12);
	read = myM2.read(0);
	printf("TEST: %x, should be 0x12\n", read);


	return 0;
}
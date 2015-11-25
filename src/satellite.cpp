#include "mem.h"
#include <cstdio>


int main()
{
	M2 M2array[4];
	M3 M3array[8];

	int read;

	M3array[0].write(0,0x12);
	M3array[1].write(0,0x12);
	M3array[2].write(0,0x12);

	read = M3array[0].read(0);
	printf("TEST: %x, should be 0x12\n", read);
	read = M3array[1].read(0);
	printf("TEST: %x, should be 0x12\n", read);
	read = M3array[2].read(0);
	printf("TEST: %x, should be 0x12\n", read);

	M2array[0].write(0,0x12);
	M2array[1].write(0,0x12);
	M2array[2].write(0,0x12);

	read = M2array[0].read(0);
	printf("TEST: %x, should be 0x12\n", read);
	read = M2array[1].read(0);
	printf("TEST: %x, should be 0x12\n", read);
	read = M2array[2].read(0);
	printf("TEST: %x, should be 0x12\n", read);
	
	return 0;
}
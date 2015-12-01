#include "mem.h"
#include <cstdio>


int main()
{
	M2 M2array[4];
	M3 M3array[8];

	int read;

    // Fill out line 0x00 of M3
	M3array[0].write(0x0,0xDE);
	M3array[1].write(0x0,0xAD);
	M3array[2].write(0x0,0xBE);
    M3array[3].write(0x0,0xEF);
    M3array[4].write(0x0,0xCA);
	M3array[5].write(0x0,0xFE);
	M3array[6].write(0x0,0xBA);
    M3array[7].write(0x0,0xBE);

    // Fill out line 0x7E of M3
    M3array[0].write(0x7E,0xDE);
	M3array[1].write(0x7E,0xAD);
	M3array[2].write(0x7E,0xBE);
    M3array[3].write(0x7E,0xEF);
    M3array[4].write(0x7E,0xCA);
	M3array[5].write(0x7E,0xFE);
	M3array[6].write(0x7E,0xBA);
    M3array[7].write(0x7E,0xBE);

    // Fill out line 0x06 of M2
    M2array[0].write(0x06,0xDE);
    M2array[1].write(0x06,0xAD);
    M2array[2].write(0x06,0xBE);
    M2array[3].write(0x06,0xEF);

    // Fill out line 0x99 of M2
    M2array[0].write(0x99,0xDE);
    M2array[1].write(0x99,0xAD);
    M2array[2].write(0x99,0xBE);
    M2array[3].write(0x99,0xEF);


    printf("\n ********** M3 **********\n");
    print_m3_memory(M3array);

    printf("\n ********** M2 **********\n");
    print_m2_memory(M2array);


	return 0;
}

/* Here is a little code to start tackling memory controller logic. 
   The functions below should read/write a 128byte packet to the memory arrays.
   They do not take into account latency or care if they overwrite or read 
   garbage. At this point they do not update the controller memory either, nor
   should they, I think that should happen in a function that calls these.
*/

#include "mem.h"
#include <cstdio>

//These are the banks of memory 
M2 M2array[4];
M3 M3array[8];

/*These are parts of the M1 memory reserved for controll memory for each 
  mem array
*/
int M2controll_mem[16];
int M3controll_mem[64];

//Data Types
#define WORD = 0
#define	QUAD = 1
#define	LONG = 2

//Function to write 128byte word to M2 memory
void M2word_write(int address, int data)
{
	int bank, row;

	for(row = address; row < address + 16; row++)
	{
		for(bank = 0; bank < 4; bank++)
		{
			M2array[0].write(row, data);
		}
	}
}

//Function to write 128byte word to M2 memory
void M2word_read(int address, int readData[64])
{
	int bank, row;
	int counter = 0;

	for(row = address; row < address + 16; row++)
	{
		for(bank = 0; bank < 4; bank++)
		{
			readData[counter] = M2array[0].read(row);
			counter++;
		}
	}
}

//Function to write 128byte word to M3 memory
void M3word_write(int address, int data)
{
	int bank, row;

	for(row = address; row < address + 8; row++)
	{
		for(bank = 0; bank < 8; bank++)
		{
			M3array[0].write(row, data);
		}
	}
}

//Function to write 128byte word to M3 memory
void M3word_read(int address, int readData[64])
{
	int bank, row;
	int counter = 0;

	for(row = address; row < address + 8; row++)
	{
		for(bank = 0; bank < 8; bank++)
		{
			readData[counter] = M3array[0].read(row);
			counter++;
		}
	}
}


int main()
{
	int address = 0;
	int data = 0xDEAD;
	int readData[64];

	M2word_write(address, data);
	M3word_write(address, data);

	M2word_read(address, readData);

	for(int i=0; i < 64; i++)
	{
		printf("Data at %d: %x\n", i, readData[i]);

	}

	M3word_read(address, readData);

	for(int i=0; i < 64; i++)
	{
		printf("Data at %d: %x\n", i, readData[i]);

	}




	return 0;
}
/* Here is a little code to start tackling memory controller logic. 
   The functions below should read/write a 128byte packet to the memory arrays.
   They do not take into account latency or care if they overwrite or read 
   garbage. At this point they do not update the controller memory either, nor
   should they, I think that should happen in a function that calls these.
*/

#include "mem.h"
#include "csv.h"
#include <cstdio>

//These are the banks of memory 
M1 MemController;
M2 M2array[4];
M3 M3array[8];

/*These are parts of the M1 memory reserved for controll memory for each 
  mem array
*/
int M2control_mem[16];
int M3control_mem[64];


//Data Types
#define WORD 0
#define	QUAD 1
#define	LONG 2

#define SEND 1
#define REQUEST 0

//Prototypes
int findFreeLine();
void M1generate(int line, int tag);

void M2word_write(int address, int data);
void M2word_read(int address, int readData[64]);
void M3word_write(int address, int data);
void M3word_read(int address, int readData[64]);

void M1generate(int line, int tag)
{
	int writeline[16];
	int tag_array[32];
	//Type bits
	writeline[16] = 0;
	writeline[15] = 0;

	//Tag bits
	for (int i = 0; i < 32; ++i) 
	{  
    	tag_array[i] = tag & (1 << i) ? 1 : 0;
	}
	
}

int main()
{
	 Info * Array1 = new Info[CSV1];   //Create array of structs

    //First csv file
    csv_1(Array1);


	int device = Array1[0].device;
	int op = Array1[0].operation;
	int ts = Array1[0].ts; 
	int tag = Array1[0].tr_data_tag;

	printf("%d, %d, %d, %d.\n", device, op, ts, tag);

//Pseudo Code
	if(op == SEND)
	{
		switch (ts)
		{
			case WORD:
				//findFreeLine needs to find lowest zero in M3valid_array.
				int writeLine = findFreeLine();
		/*		if(writeLine != 0xFF)
				{
					//M1generate needs to create the three byte line to store in M1.
					M1generate(writeLine, ts, tag);

				}
				else
				{
					//Eviction sequence HERE
					printf("M3 array is full\n");
				}

				break;				

			case QUAD:
				//Function M1generate4 needs to construct 4 M1 3 byte lines.
				int writeLines[4];
				for(int i=0; i < 3; i++)
				{
					writeLines[i] = findFreeLine();
				}
				M1generate4(writeLines);
				break;

			case LONG:
				//Function M1generate8 needs to construct 8 M1 3 byte lines.
				int writeLong[8];
				for(int i=0; i < 7; i++)
				{
					writeLong[i] = findFreeLine();
				}
				M1generate8(writeLong);
				break;
				*/
		}
	}




/*Test for R/W words at a time
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

*/


	return 0;
}

//Function to determine where to write to memory
int findFreeLine()
{
	int readline[16];
	int M3validMap = 0x50;

	for(int i=0; i < 4; i++)
	{
		MemController.read(M3validMap, readline);
		
		for(int i=0; i < 16; i++)
		{
			if(readline[i] == 0)
			{
				return i;
			}
		}	
		
		M3validMap++;
	}

	return 0xFF;
}

//Function to write 128byte word to M2 memory
void M2word_write(int address, int data)
{
	int bank, row;

	for(row = address; row < address + 16; row++)
	{
		for(bank = 0; bank < 4; bank++)
		{
			M2array[bank].write(row, data);
		}
	}
}

//Function to read 128byte word from M2 memory
void M2word_read(int address, int readData[64])
{
	int bank, row;
	int counter = 0;

	for(row = address; row < address + 16; row++)
	{
		for(bank = 0; bank < 4; bank++)
		{
			readData[counter] = M2array[bank].read(row);
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
			M3array[bank].write(row, data);
		}
	}
}

//Function to read 128byte word from M3 memory
void M3word_read(int address, int readData[64])
{
	int bank, row;
	int counter = 0;

	for(row = address; row < address + 8; row++)
	{
		for(bank = 0; bank < 8; bank++)
		{
			readData[counter] = M3array[bank].read(row);
			counter++;
		}
	}
}




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
#define WORD 128
#define	QUAD 512
#define	LONG 1024

#define SEND 1
#define REQUEST 0

//Prototypes
int findFreeLine();
void M1generate(int type, int tag, int next, int generated_m1[16]);

void M2word_write(int address, int data);
void M2word_read(int address, int readData[64]);
void M3word_write(int address, int data);
void M3word_read(int address, int readData[64]);

void M1generate(int type, int tag, int next, int generated_m1[16])
{
    // Turn type integer into bit array
    int type_bit_array[2];
    for (int bit = 0; bit < 2; ++bit) //Tag will never be more than 5 bits
	{
    	type_bit_array[1-bit] = type & (1 << bit) ? 1 : 0;
	}

    generated_m1[15] = type_bit_array[0];
	generated_m1[14] = type_bit_array[1];

    // Turn tag integer into bit array
    int tag_bit_array[5];
	for (int bit = 0; bit < 5; ++bit) //Tag will never be more than 5 bits
	{
    	tag_bit_array[4-bit] = tag & (1 << bit) ? 1 : 0;
	}

	generated_m1[13] = tag_bit_array[0];
	generated_m1[12] = tag_bit_array[1];
	generated_m1[11] = tag_bit_array[2];
	generated_m1[10] = tag_bit_array[3];
	generated_m1[9] = tag_bit_array[4];

	//generated_m1[8] is a dont care.
	generated_m1[8] = 0;

	// Turn next integer into bit array
    int next_bit_array[7];
	for (int bit = 0; bit < 7; ++bit) //Tag will never be more than 5 bits
	{
    	next_bit_array[6-bit] = next & (1 << bit) ? 1 : 0;
	}

    generated_m1[7] = next_bit_array[0];
    generated_m1[6] = next_bit_array[1];
    generated_m1[5] = next_bit_array[2];
    generated_m1[4] = next_bit_array[3];
    generated_m1[3] = next_bit_array[4];
    generated_m1[2] = next_bit_array[5];
    generated_m1[1] = next_bit_array[6];

	//generated_m1[0] is a dont care.
	generated_m1[0] = 0;
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

	//printf("%d, %d, %d, %d.\n", device, op, ts, tag);

//Pseudo Code
	if(op == SEND)
	{
		switch (ts)
		{
			case WORD:
				//findFreeLine needs to find lowest zero in M3valid_array.
				int writeLine = findFreeLine();
				M1generate(, tag);
                MemController.write(line, writeline);
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

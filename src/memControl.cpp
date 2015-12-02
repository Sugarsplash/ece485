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

#define BLOCK_OFFSET 8
#define M2BLOCK_OFFSET 16

#define INVALID_BLOCK 0xFF

// 0xFF is outside the valid address range so if next = 0xFF, there is no next
#define NO_NEXT 0xFF
#define DATA 0xBEEF
#define SEND 1
#define REQUEST 0

//Prototypes
int findFreeLine();
int findFreeLineM2();

void undoM2ValidMap(int block_num);
void undoM3ValidMap(int block_num);

int * M2findValidLines(int *array_size);
int * findValidLines(int *array_size);
int tagCompare(int *check, int length, int tag);

void M1generate(int type, int tag, int next, int generated_m1[16]);

void replaceLong();

void M2word_write(int address, int data);
void M2word_read(int address, int readData[64]);
void M3word_write(int address, int data);
void M3word_read(int address, int readData[64]);
int nextBitsToInt(int line[16]);


int main()
{
	Info * Array1 = new Info[CSV1];   //Create array of structs

    //First csv file
    csv_1(Array1);

    for (int i = 0; i < CSV1; ++ i)
    {
        int device = Array1[i].device;
    	int op = Array1[i].operation;
    	int ts = Array1[i].ts;
    	int tag = Array1[i].tr_data_tag;

        // printf("device = %d\n", device);
        // printf("op = %d\n", op);
        // printf("ts = %d\n", ts);
        // printf("tag = %d\n", tag);

        if(op == SEND)
        {
            switch (ts)
            {
                case WORD:
                {
                    //findFreeLine needs to find lowest zero in M3valid_array.
                    int free_block = findFreeLine();

                    // M3 is full
                    if(free_block == INVALID_BLOCK)
                    {
                        //Eviction sequence HERE
                        printf("\nWORD: M3 array is full\n");
                        free_block = findFreeLineM2();

                        //M2 is also full, need to send old data to satellite
                        if (free_block == INVALID_BLOCK)
                        {
				//Implement replacement policy
                        	printf("\nWORD: M2 array is full\n");
                                //need to send old data and then overwrite it
                        }

                        // Empty line found
                        else
                        {
                                int m1[16];
                                M1generate(WORD, tag, NO_NEXT, m1);
                                MemController.write(free_block, m1);
                                M2word_write(free_block * M2BLOCK_OFFSET, 0xFEEB);
                        }
                    }

                    // Empty line found
                    else
                    {
                        int m1[16];
                        M1generate(WORD, tag, NO_NEXT, m1);
                        MemController.write(free_block, m1);
                        M3word_write(free_block * BLOCK_OFFSET, DATA);
                    }
                }

                    break;

                case QUAD:
                {
                    // Get four free blocks to hold the quad
                    int free_blocks[4];

                    for (int i = 0; i < 4; ++i)
                    {
                        free_blocks[i] = findFreeLine();
                    }

                    // INVALID_BLOCK means no empty line found
                    if (free_blocks[0] == INVALID_BLOCK ||
                        free_blocks[1] == INVALID_BLOCK ||
                        free_blocks[2] == INVALID_BLOCK ||
                        free_blocks[3] == INVALID_BLOCK)
                    {
                        // Flip any valid bits back to 0 because we aren't
                        // writing to memory anymore
                        if (free_blocks[0] != INVALID_BLOCK)
                        {
                            undoM3ValidMap(free_blocks[0]);
                        }
                        if (free_blocks[1] != INVALID_BLOCK)
                        {
                            undoM3ValidMap(free_blocks[1]);
                        }
                        if (free_blocks[2] != INVALID_BLOCK)
                        {
                            undoM3ValidMap(free_blocks[2]);
                        }
                        if (free_blocks[3] != INVALID_BLOCK)
                        {
                            undoM3ValidMap(free_blocks[3]);
                        }

                        //Eviction sequence HERE
                        printf("\nQUAD: M3 array is full\n");

                	for (int i = 0; i < 4; ++i)
                    	{
                    	    free_blocks[i] = findFreeLineM2();
                    	}

                        // INVALID_BLOCK means no empty line found
                        if (free_blocks[0] == INVALID_BLOCK ||
                            free_blocks[1] == INVALID_BLOCK ||
                            free_blocks[2] == INVALID_BLOCK ||
                            free_blocks[3] == INVALID_BLOCK)
			{
                            // Flip any valid bits back to 0 because we aren't
                            // writing to memory anymore
                            if (free_blocks[0] != INVALID_BLOCK)
                            {
                                undoM2ValidMap(free_blocks[0]);
                            }
                            if (free_blocks[1] != INVALID_BLOCK)
                            {
                                undoM2ValidMap(free_blocks[1]);
                            }
                            if (free_blocks[2] != INVALID_BLOCK)
                            {
                                undoM2ValidMap(free_blocks[2]);
                            }
                            if (free_blocks[3] != INVALID_BLOCK)
                            {
                                undoM2ValidMap(free_blocks[3]);
                            }
                            printf("\nQUAD: M2 array is full\n");
			    //Implement replacement policy
			}
			
			else
			{
                            // Update M1 and write to M2
                            for (int block = 0; block < 4; ++block)
                            {
                                //printf("\nfree_blocks[%d] = %d\n", block, free_blocks[block]);
                                int m1[16];

                                int next;
                                if (block == 3)
                                {
                                    next = NO_NEXT;
                                }
                                else
                                {
                                    next = free_blocks[block+1];
                                }

                                //printf("\nnext: %d", next);
                                M1generate(QUAD, tag, next, m1);
                                MemController.write(free_blocks[block], m1);
                                M2word_write(((free_blocks[block]) - 64) * M2BLOCK_OFFSET, 0xCACA);
                            }
			}
                    }

                    // Four empty blocks found
                    else
                    {
                        // Update M1 and write to M3
                        for (int block = 0; block < 4; ++block)
                        {
                            //printf("\nfree_blocks[%d] = %d\n", block, free_blocks[block]);
                            int m1[16];

                            int next;
                            if (block == 3)
                            {
                                next = NO_NEXT;
                            }
                            else
                            {
                                next = free_blocks[block+1];
                            }

                            //printf("\nnext: %d", next);
                            M1generate(QUAD, tag, next, m1);

                            MemController.write(free_blocks[block], m1);
                            M3word_write(free_blocks[block] * BLOCK_OFFSET, 0x1111);
                        }
                    }
                }

                    break;

                case LONG:
                {
                    // Get eight free blocks to hold the long
                    int free_blocks[8];

                    for (int i = 0; i < 8; ++i)
                    {
                        free_blocks[i] = findFreeLine();
                    }

                    // INVALID_BLOCK means no empty line found
                    if (free_blocks[0] == INVALID_BLOCK ||
                        free_blocks[1] == INVALID_BLOCK ||
                        free_blocks[2] == INVALID_BLOCK ||
                        free_blocks[3] == INVALID_BLOCK ||
                        free_blocks[4] == INVALID_BLOCK ||
                        free_blocks[5] == INVALID_BLOCK ||
                        free_blocks[6] == INVALID_BLOCK ||
                        free_blocks[7] == INVALID_BLOCK )
                    {
                        // Flip any valid bits back to 0 because we aren't
                        // writing to memory anymore
                        if (free_blocks[0] != INVALID_BLOCK)
                        {
                            undoM3ValidMap(free_blocks[0]);
                        }
                        if (free_blocks[1] != INVALID_BLOCK)
                        {
                            undoM3ValidMap(free_blocks[1]);
                        }
                        if (free_blocks[2] != INVALID_BLOCK)
                        {
                            undoM3ValidMap(free_blocks[2]);
                        }
                        if (free_blocks[3] != INVALID_BLOCK)
                        {
                            undoM3ValidMap(free_blocks[3]);
                        }
                        if (free_blocks[4] != INVALID_BLOCK)
                        {
                            undoM3ValidMap(free_blocks[4]);
                        }
                        if (free_blocks[5] != INVALID_BLOCK)
                        {
                            undoM3ValidMap(free_blocks[5]);
                        }
                        if (free_blocks[6] != INVALID_BLOCK)
                        {
                            undoM3ValidMap(free_blocks[6]);
                        }
                        if (free_blocks[7] != INVALID_BLOCK)
                        {
                            undoM3ValidMap(free_blocks[7]);
                        }

                        //Eviction sequence HERE
                        printf("\nLONG: M3 array is full\n");

                	for (int i = 0; i < 8; ++i)
                    	{
                    	    free_blocks[i] = findFreeLineM2();
                    	}

                        // INVALID_BLOCK means no empty line found
                        if (free_blocks[0] == INVALID_BLOCK ||
                            free_blocks[1] == INVALID_BLOCK ||
                            free_blocks[2] == INVALID_BLOCK ||
                            free_blocks[3] == INVALID_BLOCK ||
                            free_blocks[4] == INVALID_BLOCK ||
                            free_blocks[5] == INVALID_BLOCK ||
                            free_blocks[6] == INVALID_BLOCK ||
                            free_blocks[7] == INVALID_BLOCK)
			{
                            // Flip any valid bits back to 0 because we aren't
                            // writing to memory anymore
                            if (free_blocks[0] != INVALID_BLOCK)
                            {
                                undoM2ValidMap(free_blocks[0]);
                            }
                            if (free_blocks[1] != INVALID_BLOCK)
                            {
                                undoM2ValidMap(free_blocks[1]);
                            }
                            if (free_blocks[2] != INVALID_BLOCK)
                            {
                                undoM2ValidMap(free_blocks[2]);
                            }
                            if (free_blocks[3] != INVALID_BLOCK)
                            {
                                undoM2ValidMap(free_blocks[3]);
                            }
                            if (free_blocks[4] != INVALID_BLOCK)
                            {
                                undoM2ValidMap(free_blocks[4]);
                            }
                            if (free_blocks[5] != INVALID_BLOCK)
                            {
                                undoM2ValidMap(free_blocks[5]);
                            }
                            if (free_blocks[6] != INVALID_BLOCK)
                            {
                                undoM2ValidMap(free_blocks[6]);
                            }
                            if (free_blocks[7] != INVALID_BLOCK)
                            {
                                undoM2ValidMap(free_blocks[7]);
                            }
			    printf("\nLONG: M2 array is full\n");
			    //Implement replacement policy
			    replaceLong();
			}
			
			else
			{
                            // Update M1 and write to M2
                            for (int block = 0; block < 8; ++block)
                            {
                                //printf("\nfree_blocks[%d] = %d\n", block, free_blocks[block]);
                                int m1[16];

                                int next;
                                if (block == 7)
                                {
                                    next = NO_NEXT;
                                }
                                else
                                {
                                    next = free_blocks[block+1];
                                }

                                //printf("\nnext: %d", next);
                                M1generate(LONG, tag, next, m1);
                                MemController.write(free_blocks[block], m1);
                                M2word_write(((free_blocks[block]) - 64) * M2BLOCK_OFFSET, 0xDADA);
                            }
			}
                    }

                    // 8 empty blocks found
                    else
                    {
                        // Update M1 and write to M3
                        for (int block = 0; block < 8; ++block)
                        {
                            //printf("\nfree_blocks[%d] = %d\n", block, free_blocks[block]);
                            int m1[16];

                            int next;
                            if (block == 7)
                            {
                                next = NO_NEXT;
                            }
                            else
                            {
                                next = free_blocks[block+1];
                            }

                            //printf("\nnext: %d", next);
                            M1generate(LONG, tag, next, m1);

                            MemController.write(free_blocks[block], m1);
                            M3word_write(free_blocks[block] * BLOCK_OFFSET, 0x2222);
                        }
                    }
                }
            }
        }


    	else if(op == REQUEST)
    	{
    	//search for tag in M3, if found read from M3
    		int validLine[16];
    		int requestData[64];
            int validarray_length;
            int tag_found = 0;

            int *validarray = findValidLines(&validarray_length);

            tag_found = tagCompare(validarray, validarray_length, tag);
            
            if(tag_found != -1)
            {
				if(ts == WORD)
	 			{
	 				M3word_read(tag_found * 8, requestData);
	 				//Send to device function
	 			}
	 			else if(ts == QUAD || ts == LONG)
				{
					M3word_read(tag_found * 8, requestData);
					//Send to device function
					
					MemController.read(tag_found, validLine);
					
					int nextRow;
					
					do
					{
						nextRow = nextBitsToInt(validLine);
							if(nextRow != 0x7F)
						{
							MemController.read(nextRow, validLine);
							M3word_read(nextRow * 8, requestData);
							//Send to device function
						}
					}while(nextRow != 0x7F);
				}
            }
            //Data is not in M3, search M2
            else if(tag_found == -1)
            {
				validarray = M2findValidLines(&validarray_length);

				tag_found = tagCompare(validarray, validarray_length, tag);

				if(tag_found != -1)
            	{
					if(ts == WORD)
		 			{
		 				M2word_read((tag_found - 64)* 16, requestData);
		 				//Send to device function
		 			}
		 			else if(ts == QUAD || ts == LONG)
					{
						M2word_read((tag_found - 64)* 16, requestData);
						//Send to device function
						
						MemController.read(tag_found, validLine);
						
						int nextRow;
						
						do
						{
							nextRow = nextBitsToInt(validLine);
							if(nextRow != 0x7F)
							{
								MemController.read(nextRow, validLine);
								M2word_read((tag_found - 64)* 16, requestData);
								printf("Next M2: %x\n", nextRow);
								//Send to device function
							}
						}while(nextRow != 0x7F);
					}
            	}	

 			}
    			
    			//search for tag in M2
    				//if found read from M2
    			//get from data center
    				//put into M2 and read out to device

    		
    	}
    }

    printf("\n************************* M3 *************************\n");
    print_m3_memory(M3array);

    printf("\n************************* M2 *************************\n");
    print_m2_memory(M2array);

    printf("\n************************* M1 *************************\n");
    print_m1_memory(MemController);

	return 0;
}

//Return first line in check that matches tag
int tagCompare(int *check, int length, int tag)
{
 	int tag_bit_array[5];
 	int line[16];

	for(int i=0; i < length; i++)
	{
		// Turn tag integer into bit array
		for (int bit = 0; bit < 5; ++bit) //Tag will never be more than 5 bits
		{
			tag_bit_array[4-bit] = tag & (1 << bit) ? 1 : 0;
		}
		
		MemController.read(check[i], line);

		if(tag_bit_array[0] == line[13] && \
			  tag_bit_array[1] == line[12] && \
			  tag_bit_array[2] == line[11] && \
			  tag_bit_array[3] == line[10] && \
			  tag_bit_array[4] == line[9])
		{
			return i;
		}
	}

	return -1;
}

int nextBitsToInt(int line[16])
{
	int nextRow = 0;

	for(int i=1; i < 8; i++)
   	{
   		printf("%d\n", line[i]);
    	if(line[i] == 1)
    	{
    		nextRow += 1 << i-1;
    	}
   	}
   	printf("NextRow: %d\n", nextRow);
   	return nextRow;
}

//Function to determine where to write to memory
int findFreeLineM2()
{
        int readline[16];
        int M2validMap = 0x54;

        MemController.read(M2validMap, readline);

        for(int validBit=0; validBit < 16; validBit++)
        {
                if(readline[validBit] == 0)
                {
                        readline[validBit] = 1;
                        MemController.write(M2validMap, readline);
                        return 64 + validBit;
                }
        }

        return INVALID_BLOCK;
}

//Function to determine where to write to memory
int findFreeLine()
{
	int readline[16];
	int M3validMap = 0x50;

	for(int validRow=0; validRow < 4; validRow++)
	{
		MemController.read(M3validMap, readline);

		for(int validBit=0; validBit < 16; validBit++)
		{
			if(readline[validBit] == 0)
			{
				readline[validBit] = 1;
				MemController.write(M3validMap, readline);
				return 16 * validRow + validBit;
			}
		}

		M3validMap++;
	}

	return INVALID_BLOCK;
}

void undoM2ValidMap(int block_num)
{
    int readline[16];
    int M2validMap = 0x54;

    int M2validMapIndex = block_num % 16;

    MemController.read(M2validMap, readline);

    readline[M2validMapIndex] = 0;  // Set valid bit back to 0

    MemController.write(M2validMap, readline);
}

void undoM3ValidMap(int block_num)
{
    int readline[16];
    int M3validMap = 0x50;

    int M3validMapRow = M3validMap + (block_num / 16);
    int M3validMapIndex = block_num % 16;

    MemController.read(M3validMapRow, readline);

    readline[M3validMapIndex] = 0;  // Set valid bit back to 0

    MemController.write(M3validMapRow, readline);
}

int * M2findValidLines (int *array_size)
{
	int * validarray = NULL;
	int size = 0;
	int readline[16];
	int M2validMap = 0x54;

	MemController.read(M2validMap, readline);

	for(int validBit=0; validBit < 16; validBit++)
	{
		if(readline[validBit] == 1)
		{
			size++;
		}
	}

	validarray = new int[size];

    *array_size = size;

	int counter = 0;
	M2validMap = 0x54;

	MemController.read(M2validMap, readline);

	for(int validBit=0; validBit < 16; validBit++)
	{
		if(readline[validBit] == 1)
		{
			validarray[counter] = 64 + validBit;
			counter++;
		}
	}

	return validarray;
}

int * findValidLines(int *array_size)
{
	int * validarray = NULL;
	int size = 0;
	int readline[16];
	int M3validMap = 0x50;

	for(int validRow=0; validRow < 4; validRow++)
	{
		MemController.read(M3validMap, readline);

		for(int validBit=0; validBit < 16; validBit++)
		{
			if(readline[validBit] == 1)
			{
				size++;
			}
		}

		M3validMap++;
	}

	validarray = new int[size];

    *array_size = size;

	int counter = 0;
	M3validMap = 0x50;

	for(int validRow=0; validRow < 4; validRow++)
	{
		MemController.read(M3validMap, readline);

		for(int validBit=0; validBit < 16; validBit++)
		{
			if(readline[validBit] == 1)
			{
				validarray[counter] = validRow * 16 + validBit;
				counter++;
			}
		}

		M3validMap++;
	}

	return validarray;
}

void M1generate(int type, int tag, int next, int generated_m1[16])
{
    // Turn type integer into bit array
    int type_bit_array[2];
    for (int bit = 0; bit < 2; ++bit)
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
	for (int bit = 0; bit < 7; ++bit)
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

//This function finds all rows in M1 from 0x00 to 0x50 that
//contain a transaction size of 1024 bytes
int * findValidType(int *array_size)
{
	int * validtype = NULL;
	int size = 0;
	int type = 0;
	int tag_line = 0;
	int readline[16];
	int M2start = 0x40;

	//Find valid types through M3 and M2
	for(int Row=0; Row < 0x50; Row++)
	{
		//Searching through M3 and M2
		MemController.read(Row, readline);
		type = (readline[15] << 1) + readline[14];
		if (type == 0x3)
			size++;
	}

	validtype = new int[size];

    *array_size = size;

	int counter = 0;

	for(int Row=0; Row < 0x50; Row++)
	{
		//Fill in array which area to go to when type is valid
		MemController.read(Row, readline);
		type = (readline[15] << 1) + readline[14];

		if(type == 0x3)
		{
			validtype[counter] = Row;
			Row = Row + 7;
			counter++;
		}
	}

	return validtype;
}

void replaceLong()
{
	int readline[16];
        int validtype_length;
        int *validtype = findValidType(&validtype_length);

	int tag_line[validtype_length];
	int smallest_tag;
	int row_to_evict;

	int data_to_satellite[64];

	for(int i=0; i < validtype_length; i++)
	{
		MemController.read(validtype[i], readline);

		tag_line[i] = 	(readline[13] << 4) +
				(readline[12] << 3) +
				(readline[11] << 2) +
				(readline[10] << 1) +
				(readline[9]);
		if (i = 0)
			smallest_tag = tag_line[0];
		else
		{
			if (smallest_tag > tag_line[i])
			{
				smallest_tag = tag_line[i];
				row_to_evict = i;
			}
		}
	}

	//Check whether we need to evict from M3 or M2
	if (validtype[row_to_evict] < 0x64) //M3
	{
		M3word_read(row_to_evict, data_to_satellite);
	}
	else //(0x80 > validtype[row_to_evict] > 0x64), M2
	{
		M2word_read(row_to_evict, data_to_satellite);
	}

	//Send data to satellite 2 bytes at a time at a speed of 1333333 clock cycles
		//Sending 1024 bytes needs 682666666 clock cycles
		//This will take (10 ns) * 682666666 = 6.826 seconds
		//The link cost is $(1/60) per second
		//This transmission will cost us $0.114 or about 11 cents
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

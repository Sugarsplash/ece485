/* Here is a little code to start tackling memory controller logic.
   The functions below should read/write a 128byte packet to the memory arrays.
   They do not take into account latency or care if they overwrite or read
   garbage. At this point they do not update the controller memory either, nor
   should they, I think that should happen in a function that calls these.
*/

#include "mem.h"
#include "csv.h"
#include <cstdio>

//Global latency counter
extern double LATENCY_COUNTER;
//Precalculated device access latency due to bandwidth of connection
#define BYTE_ACCESS_128 18619
#define BYTE_ACCESS_512 74473
#define BYTE_ACCESS_1KB 148945

//Precalculated satellite access time
#define SAT_ACCESS_1KB 682666666

//This will track our unit cost
#define UNIT_COST 114

//This will track our communication cost
float COMM_COST = 0;

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

//Offsets to access next word in memory arrays
#define BLOCK_OFFSET 8
#define M2BLOCK_OFFSET 16
//Sequence to denote invalid block
#define INVALID_BLOCK 0xFF

// 0xFF is outside the valid address range so if next = 0xFF, there is no next
#define NO_NEXT 0xFF
//Encoding for send and request commands
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
int * findValidType(int *array_size);
int * replaceLong(int *index_to_evict, int tag);
void M2word_write(int address, int data);
void M2word_read(int address, int readData[64]);
void M3word_write(int address, int data);
void M3word_read(int address, int readData[64]);
int nextBitsToInt(int line[16]);

//Begin Simulation
int main()
{
	//Info contains transaction information from csv file
	Info * Array1 = new Info[CSV1];   //Create array of structs

    //First csv file
    csv_1(Array1);

    for (int i = 0; i < CSV1; ++ i)
    {
        int device = Array1[i].device;
    	int op = Array1[i].operation;
    	int ts = Array1[i].ts;
    	int tag = Array1[i].tr_data_tag;
    	
    	//Logic for Send transaction
        if(op == SEND)
        {
        	//Determine action based on transaction size
            switch (ts)
            {
            	//128byte
                case WORD:
                {
                    //findFreeLine needs to find lowest zero in M3valid_array.
                    int free_block = findFreeLine();

                    // M3 is full
                    if(free_block == INVALID_BLOCK)
                    {
            			//find free lines in M2
                        free_block = findFreeLineM2();
                        int m1[16];
                        M1generate(WORD, tag, NO_NEXT, m1);
                        MemController.write(free_block, m1);
                        M2word_write(free_block * M2BLOCK_OFFSET, tag);
                   		LATENCY_COUNTER += BYTE_ACCESS_128;

                    }

                    // Empty line found
                    else
                    {
                        int m1[16];
                        M1generate(WORD, tag, NO_NEXT, m1);
                        MemController.write(free_block, m1);
                        M3word_write(free_block * BLOCK_OFFSET, tag);
						LATENCY_COUNTER += BYTE_ACCESS_128;
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
                        //QUAD: M3 array is full

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
                            //QUAD: M2 array is full
			}

			else
			{
                            // Update M1 and write to M2
                            for (int block = 0; block < 4; ++block)
                            {
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

                                M1generate(QUAD, tag, next, m1);
                                MemController.write(free_blocks[block], m1);
                                M2word_write(((free_blocks[block]) - 64) * M2BLOCK_OFFSET, tag);
								LATENCY_COUNTER += BYTE_ACCESS_128;

                            }
			}
                    }

                    // Four empty blocks found
                    else
                    {
                        // Update M1 and write to M3
                        for (int block = 0; block < 4; ++block)
                        {
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

                            M1generate(QUAD, tag, next, m1);

                            MemController.write(free_blocks[block], m1);
                            M3word_write(free_blocks[block] * BLOCK_OFFSET, tag);
 							LATENCY_COUNTER += BYTE_ACCESS_128;

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
                        //LONG: M3 array is full

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
			    //LONG: M2 array is full
				//Implement replacement policy replaceLong();
				int index = 0;
				int * replace_blocks = replaceLong(&index, tag);
				// Update M1 and write to M2
				for (int block = 0; block < 8; ++block)
				{
					int m1[16];

					int next;
					if (block == 7)
					{
						next = NO_NEXT;
					}
					else
					{
						next = replace_blocks[index] + block + 1;
					}

					M1generate(LONG, tag, next, m1);
					MemController.write(replace_blocks[index] + block, m1);
				}
			}

			else
			{
                            // Update M1 and write to M2
                            for (int block = 0; block < 8; ++block)
                            {
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

                                M1generate(LONG, tag, next, m1);
                                MemController.write(free_blocks[block], m1);
                                M2word_write(((free_blocks[block]) - 64) * M2BLOCK_OFFSET, tag);
								LATENCY_COUNTER += BYTE_ACCESS_128;

                            }
						}
                    }

                    // 8 empty blocks found
                    else
                    {
                        // Update M1 and write to M3
                        for (int block = 0; block < 8; ++block)
                        {
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

                            M1generate(LONG, tag, next, m1);

                            MemController.write(free_blocks[block], m1);
                            M3word_write(free_blocks[block] * BLOCK_OFFSET, tag);
							LATENCY_COUNTER += BYTE_ACCESS_128;

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
					LATENCY_COUNTER += BYTE_ACCESS_128;
	 			}
	 			else if(ts == QUAD || ts == LONG)
				{
					M3word_read(tag_found * 8, requestData);
					LATENCY_COUNTER += BYTE_ACCESS_128;

					MemController.read(tag_found, validLine);

					int nextRow;

					do
					{
						nextRow = nextBitsToInt(validLine);
							if(nextRow != 0x7F)
						{
							MemController.read(nextRow, validLine);
							M3word_read(nextRow * 8, requestData);
							LATENCY_COUNTER += BYTE_ACCESS_128;
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

						LATENCY_COUNTER += BYTE_ACCESS_128;
		 			}
		 			else if(ts == QUAD || ts == LONG)
					{
						M2word_read((tag_found - 64)* 16, requestData);

						LATENCY_COUNTER += BYTE_ACCESS_128;

						MemController.read(tag_found, validLine);

						int nextRow;

						do
						{
							nextRow = nextBitsToInt(validLine);
							if(nextRow != 0x7F)
							{
								MemController.read(nextRow, validLine);
								M2word_read((tag_found - 64)* 16, requestData);
								LATENCY_COUNTER += BYTE_ACCESS_128;
							}
						}while(nextRow != 0x7F);
					}
            	}
            	//Not in memory, get DataCenter
            	else
            	{
				//Add 450 clock cycles to latency calculation
					LATENCY_COUNTER += SAT_ACCESS_1KB  + BYTE_ACCESS_1KB  + 450;
					COMM_COST += 0.114;
            	}

 			}

    	}
    }

    printf("\n************************* M3 *************************\n");
    print_m3_memory(M3array);

    printf("\n************************* M2 *************************\n");
    print_m2_memory(M2array);

    printf("\n************************* M1 *************************\n");
    print_m1_memory(MemController);

    printf("Simulation Results:\nUnit Cost: $114.00\n\t%e cycles, and communication cost was $%0.2f\n", LATENCY_COUNTER, COMM_COST);

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
    	if(line[i] == 1)
    	{
    		nextRow += 1 << i-1;
    	}
   	}
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
		if (type == 0x2)
		{
			size++;
			Row = Row + 7;
		}
	}

	validtype = new int[size];

    *array_size = size;

	int counter = 0;

	for(int Row=0; Row < 0x50; Row++)
	{
		//Fill in array which area to go to when type is valid
		MemController.read(Row, readline);
		type = (readline[15] << 1) + readline[14];

		if(type == 0x2)
		{
			validtype[counter] = Row;
			Row = Row + 7;
			counter++;
		}
	}

	return validtype;
}

int * replaceLong(int *index_to_evict, int tag)
{
	int readline[16];
        int validtype_length;
        int *validtype = findValidType(&validtype_length);

	int tag_line[validtype_length];
	int smallest_tag;

	int data_to_satellite[64];

	for(int i=0; i < validtype_length; i++)
	{
		MemController.read(validtype[i], readline);

		tag_line[i] = 	(readline[13] << 4) + \
				(readline[12] << 3) + \
				(readline[11] << 2) + \
				(readline[10] << 1) + \
				(readline[9]);
		if (i == 0)
		{
			smallest_tag = tag_line[0];
			*index_to_evict = 0;
		}
		else
		{
			if (smallest_tag > tag_line[i])
			{
				smallest_tag = tag_line[i];
				*index_to_evict = i;
			}
		}
	}

	//Check whether we need to evict from M3 or M2
	if (validtype[*index_to_evict] < 0x40) //M3
	{
		M3word_read(validtype[*index_to_evict], data_to_satellite);

		//Add 200 clock cycles to latency for getting to data center
		LATENCY_COUNTER += SAT_ACCESS_1KB + 200;
		COMM_COST += 0.114;
	}
	else //(0x50 > validtype[index_to_evict] > 0x40), M2
	{
		M2word_read(validtype[*index_to_evict], data_to_satellite);

		//Add 200 clock cycles to latency for getting to data center
		LATENCY_COUNTER += SAT_ACCESS_1KB + 200;
		COMM_COST += 0.114;
	}

	//Send data to satellite 2 bytes at a time at a speed of 1333333 clock cycles
		//Sending 1024 bytes needs 682666666 clock cycles
		//This will take (10 ns) * 682666666 = 6.826 seconds
		//The link cost is $(1/60) per second
		//This transmission will cost us $0.114 or about 11 cents

	//Start overwriting the data
	if (validtype[*index_to_evict] < 0x40) //M3
	{
		for(int i=0; i < 8; i++)
		{
			M3word_write((validtype[*index_to_evict]+i)*BLOCK_OFFSET, tag);
		}
	}
	else //(0x50 > validtype[index_to_evict] > 0x40), M2
	{
		for(int i=0; i < 8; i++)
		{
			M2word_write((validtype[*index_to_evict]+i)*M2BLOCK_OFFSET, tag);
		}
	}

	return validtype;

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

	LATENCY_COUNTER += 146;
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

	LATENCY_COUNTER += 146;
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

	LATENCY_COUNTER += 134;
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

	LATENCY_COUNTER += 134;
}

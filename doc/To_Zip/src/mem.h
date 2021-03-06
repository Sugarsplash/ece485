#ifndef MEM_H
#define MEM_H

using namespace std;
//This will be used to keep track of latency

/* This M1 class will be used to construct a single M1 Memory
	 The memory will be set as a 2D array in order to access individual bits.
*/

class M1
{
public:
	int memlock;

    M1(void);

	void write(int row, int data[16]);
	void read(int row, int readData[16]);

private:
	int mem_matrix[128][16];
};



/* This M2 class will be used to construct the M2 x 4 Memory Array
	Should be a class that has rows of memory as members and read and write as
	methods. This memory array consists of 4 individual M2 memory chips. Each
	chip is 2bytes wide by 256 rows. Put together, there should be 4 memory
	banks in parallel that each have a 16bit data bus. The memory should store
	the following packets in the following way:

			16 rows for 128byte packet
			64 rows for 512byte packet
			128 rows for 1Kbyte packet

	Total memory array capacity is 2Kb.


	Other considerations:
		-Memory is aligned in increments of 16 rows, i.e. 0x0, 0x16, 0x32 etc.
		-Memory supports noncontiguous memory access but must be aligned.
*/


class M2
{
public:
	int memlock;

    M2(void);

	void write(int row, int data);
	int read(int row);

private:
	int mem_matrix[256];
};

/* This M3 class will be used to construct the M3 x 8 Memory Array
	Should be a class that has rows of memory as members and read and write as
	methods. This memory array consists of 8 individual M3 memory chips. Each
	chip is 2bytes wide by 512 rows. Put together, there should be 8 memory
	banks in parallel that each have a 16bit data bus. The memory should store
	the following packets in the following way:

			8 rows for 128byte packet
			32 rows for 512byte packet
			64 rows for 1Kbyte packet

	Total memory array capacity is 8Kb.


	Other considerations:
		-Memory is aligned in increments of 8 rows, i.e. 0x0, 0x8 0x16 etc.
		-Memory supports noncontiguous memory access but must be aligned.
*/

class M3
{
public:
	int memlock;

    M3(void);

	void write(int row, int data);
	int read(int row);

private:
	int mem_matrix[512];
};

//Prototypes
void print_m1_memory(M1 M1array);
void print_m2_memory(M2 *M2array);
void print_m3_memory(M3 *M3array);
void print_m3_valid_map(M1 M1array);
void print_m2_valid_map(M1 M1array);

#endif

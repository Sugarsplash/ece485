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
		-Memory is aligned in increments of 16 rows, i.e. 0x0, 0x15, 0x31 etc.
		-Memory supports noncontiguous memory access but must be aligned.
*/

using namespace std;

class M2
{
public:
	int memlock;

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
		-Memory is aligned in increments of 8 rows, i.e. 0x0, 0x7, 0x15 etc.
		-Memory supports noncontiguous memory access but must be aligned.
*/

class M3
{
public:
	int memlock;

	void write(int row, int data);
	int read(int row);

private:
	int mem_matrix[512];
};
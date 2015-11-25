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

#include <cstdio>

using namespace std;

class M3
{
public:
	int memlock;

	void write(int row, int data);
	int read(int row);

private:
	int mem_matrix[512];
};

void M3::write(int row, int data)
{
	mem_matrix[row] = data;
}

int M3::read(int row)
{
	return mem_matrix[row];
}

int main()
{

	M3 myM3;

	int read;

	myM3.write(0,0x12);
	read = myM3.read(0);
	printf("TEST: %x, should be 0x12\n", read);


	return 0;


}
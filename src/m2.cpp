/* M2 x 4 Memory Array
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
#include <cstdio>

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

void M2::write(int row, int data)
{
	mem_matrix[row] = data;
}

int M2::read(int row)
{
	return mem_matrix[row];
}

int main()
{

	M2 myM2;

	int read;

	myM2.write(0,0x12);
	read = myM2.read(0);
	printf("TEST: %x, should be 0x12\n", read);


	return 0;


}
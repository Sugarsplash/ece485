#include "mem.h"

void M2::write(int row, int data)
{
	mem_matrix[row] = data;
}

int M2::read(int row)
{
	return mem_matrix[row];
}

void M3::write(int row, int data)
{
	mem_matrix[row] = data;
}

int M3::read(int row)
{
	return mem_matrix[row];
}

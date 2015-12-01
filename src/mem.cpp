#include "mem.h"
#include <cstdio>

using namespace std;
M1::M1(void)
{
    for (int i = 0; i < 128; ++i)
    {
        mem_matrix[i] = 0;
    }
}

void M1::write(int row, int data)
{
	mem_matrix[row] = data;
}

void M1::read(int row, int readData[16])
{
	mem_matrix[row];
}

M2::M2(void)
{
    for (int i = 0; i < 256; ++i)
    {
        mem_matrix[i] = 0;
    }
}

void M2::write(int row, int data)
{
	mem_matrix[row] = data;
}

int M2::read(int row)
{
	return mem_matrix[row];
}

M3::M3(void)
{
    for (int i = 0; i < 512; ++i)
    {
        mem_matrix[i] = 0;
    }
}

void M3::write(int row, int data)
{
	mem_matrix[row] = data;
}

int M3::read(int row)
{
	return mem_matrix[row];
}

void print_m1_memory(M1 *M1array)
{
    for (int i = 0; i < 128; ++i)
    {
        printf("%02X\t", i);
        for (int j = 0; j < 4; ++j)
        {
            printf("%04X ", M1array[j].read(i));
        }

        printf("\n");
    }
}

void print_m2_memory(M2 *M2array)
{
    for (int i = 0; i < 256; ++i)
    {
        printf("%02X\t", i);
        for (int j = 0; j < 4; ++j)
        {
            printf("%04X ", M2array[j].read(i));
        }

        printf("\n");
    }
}

void print_m3_memory(M3 *M3array)
{
    for (int i = 0; i < 512; ++i)
    {
        printf("%04X\t", i);
        for (int j = 0; j < 8; ++j)
        {
            printf("%04X ", M3array[j].read(i));
        }

        printf("\n");
    }
}

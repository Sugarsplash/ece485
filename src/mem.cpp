#include "mem.h"
#include <cstdio>

using namespace std;
M1::M1(void)
{
        for (int i = 0; i < 128; ++i)
        {
            for (int j = 0; j < 16; ++j)
            {
                mem_matrix[i][j] = 0;
            }
        }
}

void M1::write(int row, int data[16])
{
    for (int i = 0; i < 16; ++i)
    {
        mem_matrix[row][i] = data[i];
    }

}

void M1::read(int row, int readData[16])
{
    for (int i = 0; i < 16; ++i)
    {
        readData[i] = mem_matrix[row][i];
    }
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

void print_m1_memory(M1 M1array)
{
    for (int line = 0; line < 128; ++line)       // Line
    {
        printf("%02X\t", line);

        int data[16];

        M1array.read(line, data);

        for (int bit = 0; bit < 16; ++bit)
        {
            printf("%d", data[bit]);
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

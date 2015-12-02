#include "mem.h"
#include <cstdio>

using namespace std;
double LATENCY_COUNTER = 0;

M1::M1(void)
{
	//Constructor: zero out the memory
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
	//Add 1 to the latency of the global clock
    }

    LATENCY_COUNTER += 2;

}

void M1::read(int row, int readData[16])
{
    for (int i = 0; i < 16; ++i)
    {
        readData[i] = mem_matrix[row][i];
	//Add 1 to the latency of the global clock
    }

    LATENCY_COUNTER += 2;

}

M2::M2(void)
{
    //Constructor: zero out the memory
    for (int i = 0; i < 256; ++i)
    {
        mem_matrix[i] = 0;
    }
}

void M2::write(int row, int data)
{
	mem_matrix[row] = data;
	//Add 8 to the latency of the global clock
}

int M2::read(int row)
{
	return mem_matrix[row];
	//Add 8 to the latency of the global clock
}

M3::M3(void)
{
    //Constructor: zero out the memory
    for (int i = 0; i < 512; ++i)
    {
        mem_matrix[i] = 0;
    }
}

void M3::write(int row, int data)
{
	mem_matrix[row] = data;
	//Add 15 to the latency of the global clock
}

int M3::read(int row)
{
	return mem_matrix[row];
	//Add 15 to the latency of the global clock
}

void print_m1_memory(M1 M1array)
{
    for (int line = 0; line < 0x50; ++line)       // Line
    {
        printf("%02X\t", line);

        int data[16];

        M1array.read(line, data);

        for (int bit = 15; bit >= 0; --bit)
        {
            printf("%d", data[bit]);

            // Add spacing between fields (type, x, tag, next, x)
            if (bit == 14 || bit == 8 || bit == 9 || bit == 1)
            {
                printf(" ");
            }
        }

        printf("\n");
    }
}

void print_m3_valid_map(M1 M1array)
{
    for (int line = 0x50; line < 0x54; ++line)       // Line
    {
        printf("%02X\t", line);

        int data[16];

        M1array.read(line, data);

        for (int bit = 15; bit >= 0; --bit)
        {
            printf("%d ", data[bit]);
        }

        printf("\n");
    }
}

void print_m2_valid_map(M1 M1array)
{
    int line = 0x54;

    printf("%02X\t", line);

    int data[16];

    M1array.read(line, data);

    for (int bit = 15; bit >= 0; --bit)
    {
        printf("%d ", data[bit]);
    }
}

void print_m2_memory(M2 *M2array)
{
    for (int i = 0; i < 256; ++i)
    {
        // Add new line between blocks for visual purposes
        if (!(i % 8))
        {
            printf("\n");
        }

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
        // Add new line between blocks for visual purposes
        if (!(i % 8))
        {
            printf("\n");
        }

        printf("%04X\t", i);
        for (int j = 0; j < 8; ++j)
        {
            printf("%04X ", M3array[j].read(i));
        }

        printf("\n");
    }
}

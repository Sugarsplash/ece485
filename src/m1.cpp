/*
	This is a single M1 memory that is used as a memory controller
	for the other M2 and M3 memories. It will receive a signal
	indicating whether the operation is a read or write. It will
	be given the size of the data as well its tag.

	This memory controller needs to keep track of the tags
	currently used in the data (it is assumed that tags will
	have the same transaction sizes assigned to them). It will
	also need to know the location in memory for the row it
	needs to access when reading/writing (the transaction size
	will tell us how much memory needs to be accessed while the
	only piece we need to know is the first row access).
*/

#include <stdio.h>
#include <stdlib.h>

/* 

		Arguments given to memory controller

	SEND or REQUEST
	ts
	tr_data_tag

*/

/*int main(int argc, char argv[]) {
    // Keep track of tags
    // Keep track of data rows for a tag (based on transaction size)

    return 0;
}*/

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
*/


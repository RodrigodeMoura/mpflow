/*
	power_of_two.c	WJ109
*/

#include "power_of_two.h"

/*
	quick test to see if x is a power of two
	thanks to wikipedia
*/
int is_power_of_two(int x) {
	if (x <= 0)
		return 0;

	if ((x & (x - 1)) == 0)
		return 1;

	return 0;
}

/*
	get next power of two, without any round(log2) crap
	thanks to wikipedia
*/
int next_power_of_two(int x) {
int i;

	x--;
	for(i = 1; i < sizeof(int) * 8; i <<= 1)
		x = x | (x >> i);
	x++;
	return x;
}

/* EOB */

#include <stdio.h>
#include "libfunc.h"

//TASK:
// Read iio.h (see slack) and find a way to read the raw attribute from the voltage0 channel from
// <your device here> by using your board URI and libiio functions. Print the raw value
// that was read to stdout (printf) and you can go home

int main() {
	printf("Hello, World!\n");

	// The number of backends available
	// printf("%d\n", iio_get_backends_count());

	func();
	return 0;
}
#include <stdio.h>
#include "libad5592r.h"

int main(const int argc, const char *argv[]) {
	const char *ip;
	const char my_ip[] = "10.76.84.22";
	if(argc > 1) {
		ip = argv[1];
	} else {
		ip = my_ip;
	}
	read_ad5592r(ip);

	return 0;
}

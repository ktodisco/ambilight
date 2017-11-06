#include "lpd8806.h"

#include <cmath>
#include <cstdio>
#include <sys/time.h>

#define LEDCOUNT 56

int main() {
	LPD8806 strip(LEDCOUNT, "/dev/spidev0.0");

	strip.fill(70, 70, 70);

	while (true) {
		strip.update();
	}

	return 0;
}

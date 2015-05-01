#include "tools.h"

const unsigned int NS_IN_S = 1000 * 1000 * 1000;

void rand_sleep(unsigned int max) {
	unsigned int ms = rand() % max;
	milli_sleep(ms);
}

void milli_sleep(unsigned int milliseconds){
	nano_sleep(milliseconds * 1000 * 1000);
}

void micro_sleep(unsigned int microseconds){
	nano_sleep(microseconds * 1000);
}

void nano_sleep(unsigned int nanoseconds){
	struct timespec t;
	struct timespec rem;
	t.tv_sec = 0;
	while (nanoseconds >= NS_IN_S) {
		++t.tv_sec;
		nanoseconds -= NS_IN_S;
	}
	t.tv_nsec = nanoseconds;
	rem.tv_sec = 0;
	rem.tv_nsec = 0;
	nanosleep(&t, &rem);
}

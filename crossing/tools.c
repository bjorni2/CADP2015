#include "tools.h"

struct timespec t;
struct timespec rem;

void milli_sleep(unsigned int milliseconds){
	nano_sleep(milliseconds * 1000 * 1000);
}

void micro_sleep(unsigned int microseconds){
	nano_sleep(microseconds * 1000);
}

void nano_sleep(unsigned int nanoseconds){
	t.tv_sec = 0;
	t.tv_nsec = nanoseconds;
	rem.tv_sec = 0;
	rem.tv_nsec = 0;
	nanosleep(&t, &rem);
}
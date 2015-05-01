#include "pedestrian.h"

void * enter_p(void * argp)
{
    try_cross_s(PEDESTRIAN);
    return argp;
}

void * enter_p_c(void * argp) {
	try_cross_ped(rand() % 2);
	return argp;
}

void * enter_p_b(void * argp) {
// TODO: randomize direction
//	try_cross_c();
	return argp;
}


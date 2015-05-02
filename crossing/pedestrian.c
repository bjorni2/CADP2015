#include "pedestrian.h"

void * enter_p(void * argp)
{
    try_cross_s(PEDESTRIAN);
    return argp;
}

void * enter_p_c(void * argp) {
	try_cross_c(PEDESTRIAN, rand() % 2);
	return argp;
}

void * enter_p_b(void * argp) {
	try_cross_b(PEDESTRIAN, rand() % 2);
	return argp;
}


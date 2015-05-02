#include "vehicle.h"

void * enter_v(void * argp) {
    try_cross_s(VEHICLE);
    return argp;
}

void * enter_v_c(void * argp) {
	try_cross_c(VEHICLE, rand() % 2);
	return argp;
}

void * enter_v_b(void * argp) {
	try_cross_b(VEHICLE, rand() % 2);
	return argp;
}


#include "vehicle.h"

void * enter_v(void * argp)
{
    try_cross_s(VEHICLE);
    return argp;
}

inline void veh_do_crossing() {
	// TODO: sleep 200 ms
	done_crossing_s(VEHICLE);
}


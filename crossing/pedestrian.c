#include "pedestrian.h"

void * enter_p(void * argp)
{
    try_cross_s(PEDESTRIAN);
	// either I cross, or I can't and I'm suspended
	ped_do_crossing();
    return argp;
}

inline void ped_do_crossing() {
	// TODO:
	// sleep 200 ms
	done_crossing_s(PEDESTRIAN);
}

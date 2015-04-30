#include "vehicle.h"

void * enter_v(void * argp)
{
    try_cross_s(VEHICLE);
    return argp;
}

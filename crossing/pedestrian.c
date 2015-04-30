#include "pedestrian.h"

void * enter_p(void * argp)
{
    try_cross_s(1);
    return argp;
}

#pragma once
#include <time.h>

extern struct timespec t;
extern struct timespec rem;

void milli_sleep(unsigned int milliseconds);
void micro_sleep(unsigned int microseconds);
void nano_sleep(unsigned int nanoseconds);
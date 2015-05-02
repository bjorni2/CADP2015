#pragma once
#include <signal.h>
#include <stdlib.h>
#include <time.h>

extern volatile sig_atomic_t PROG_RUNNING;

void rand_sleep();
void milli_sleep(unsigned int milliseconds);
void micro_sleep(unsigned int microseconds);
void nano_sleep(unsigned int nanoseconds);

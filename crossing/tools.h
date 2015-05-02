#pragma once
#include <semaphore.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include "logger.h"

extern volatile sig_atomic_t PROG_RUNNING;

void rand_sleep();
void milli_sleep(unsigned int milliseconds);
void micro_sleep(unsigned int microseconds);
void nano_sleep(unsigned int nanoseconds);

// semaphore wrappers
#define _LSEM_WAIT(x) Lsem_wait(x, #x, __LINE__)
#define _LSEM_POST(x) Lsem_post(x, #x, __LINE__)
int Lsem_wait(sem_t * sem, const char * name, int lineno);
int Lsem_post(sem_t * sem, const char * name, int lineno);

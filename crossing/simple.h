#pragma once

#define PEDESTRIAN 1
#define VEHICLE    2

#define MSG_SIZE   256

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "logger.h"

extern struct simple {
	unsigned int v_waiting;
	unsigned int p_waiting;
	unsigned int v_crossing;
	unsigned int p_crossing;

} state_s;

extern bool started_s;
extern unsigned int K_s;

void start_s(unsigned int k, size_t MAX_SPAWNS);
void try_cross(unsigned int type);

// unsigned int MAX_SPAWNS
void * spawner_s(void * argp);


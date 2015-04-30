#pragma once

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "logger.h"
#include "pedestrian.h"
#include "vehicle.h"

#define MSG_SIZE   256

#define PEDESTRIAN 0
#define VEHICLE    1
#define MAX_TYPE   2

extern struct simple {
	unsigned int waiting [2];
	unsigned int crossing [2];
	unsigned int turn;
//	bool timeout;
} state_s;

extern bool started_s;
extern unsigned int K_s;
extern sem_t light_s;
extern sem_t lord_s;

void start_s(unsigned int k, size_t MAX_SPAWNS);
void try_cross_s(unsigned int type);

void * spawner_s(void * argp);

void done_crossing_s(unsigned int type);
// inline functions
inline bool can_cross(unsigned int type);
inline void cross(unsigned int type);
inline void waiting(unsigned int type);
inline void not_waiting(unsigned int type);

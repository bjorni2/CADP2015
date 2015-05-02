#pragma once 

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "logger.h"
#include "tools.h"
#include "pedestrian.h"
#include "vehicle.h"

#define MSG_SIZE   256

#define PEDESTRIAN 0
#define VEHICLE    1
#define MAX_TYPE   2

#define VERTICAL   0
#define HORIZONTAL 1
#define DIAGONAL   2

struct busy {
	unsigned int crossing_n[2];
	unsigned int crossing_e[2];
	unsigned int waiting_n[2];
	unsigned int waiting_e[2];
	// TODO: others
} state_b;

extern bool started_b;
extern unsigned int K_b;
extern sem_t light_b;
// TODO: these semaphores!! Also change in busy.c
extern sem_t turn_v[MAX_TYPE];
extern sem_t turn_h[MAX_TYPE];

void start_b(unsigned int k, size_t MAX_SPAWNS);

void * spawner_b(void * argp);

void signal_b();

inline void inc_bross_c(unsigned int type, unsigned int dir);
inline void done_inc_bross_c(unsigned int type, unsigned int dir);
inline void waiting_b(unsigned int type, unsigned int dir);
inline void not_waiting_b(unsigned int type, unsigned int dir);

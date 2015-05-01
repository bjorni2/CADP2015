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

struct crossing {
	unsigned int crossing_n[2];
	unsigned int crossing_e[2];
	unsigned int waiting_n[2];
	unsigned int waiting_e[2];
} state_c;

extern bool started_s;
extern unsigned int K_s;
extern sem_t light_s;
extern sem_t turn[MAX_TYPE];

void try_cross_ped(unsigned int dir);
void try_cross_veh(unsigned int dir);

void start_c(unsigned int k, size_t MAX_SPAWNS);

void signal_c(unsigned int type);

inline void inc_cross_c(unsigned int type);
inline void done_crossing_c(unsigned int type);
inline void waiting(unsigned int type);
inline void not_waiting(unsigned int type);

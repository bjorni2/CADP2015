#pragma once 

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#define _LL_SEM_WAIT(x) _LSEM_WAIT(x) ; log_sem_c(NULL)
#define _LL_SEM_POST(x) _LSEM_POST(x) ; log_sem_c(NULL)

struct crossing {
	unsigned int crossing[2][MAX_TYPE];
	unsigned int waiting[2][MAX_TYPE];
	unsigned int k[2]; // beware dragons
	bool last[2]; // 0: chosen p-dir, 1: p vs v winner
	bool last_pv;
} state_c;

extern bool started_c;
extern unsigned int K_c;
extern sem_t light_c;
extern sem_t turn_c[2][MAX_TYPE];

void try_cross_c(unsigned int type, unsigned int dir);

void start_c(unsigned int k, size_t MAX_SPAWNS);

void * spawner_c(void * argp);

void signal_c(unsigned int type, unsigned int dir);

inline void push_button_c(unsigned int dir);
inline bool leaving_crossing_c(unsigned int dir, unsigned int type);
inline bool can_cross_c(unsigned int type, unsigned int dir);
inline bool dec_k_c(unsigned int type, unsigned int dir);

inline void inc_cross_c(unsigned int type, unsigned int dir);
inline void done_inc_cross_c(unsigned int type, unsigned int dir);
inline void waiting_c(unsigned int type, unsigned int dir);
inline void not_waiting_c(unsigned int type, unsigned int dir);

void log_sem_c(const char * append);

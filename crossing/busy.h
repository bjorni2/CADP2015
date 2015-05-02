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
#define DIAGONAL   2

#define _LLB_SEM_WAIT(x) _LSEM_WAIT(x) ; log_sem_b(NULL)
#define _LLB_SEM_POST(x) _LSEM_POST(x) ; log_sem_b(NULL)

struct busy {
	unsigned int crossing[3][MAX_TYPE];
	unsigned int waiting[3][MAX_TYPE];
	unsigned int k[3]; // beware dragons
	bool last[2]; // 0: chosen p-dir, 1: p vs v winner
	bool last_pv;
} state_b;

extern bool started_b;
extern unsigned int K_b;
extern sem_t light_b;
extern sem_t turn_b[3][MAX_TYPE];

void try_cross_b(unsigned int type, unsigned int dir);

void start_b(unsigned int k, size_t MAX_SPAWNS);

void * spawner_b(void * argp);

void signal_b(unsigned int type, unsigned int dir);

inline void push_button_b(unsigned int dir);
inline bool leaving_crossing_b(unsigned int dir, unsigned int type);
inline bool can_cross_b(unsigned int type, unsigned int dir);
inline bool dec_k_b(unsigned int type, unsigned int dir);

inline void inc_cross_b(unsigned int type, unsigned int dir);
inline void done_inc_cross_b(unsigned int type, unsigned int dir);
inline void waiting_b(unsigned int type, unsigned int dir);
inline void not_waiting_b(unsigned int type, unsigned int dir);

void log_sem_b(const char * append);

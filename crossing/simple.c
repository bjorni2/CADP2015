#include "simple.h"

struct simple state_s = { .turn = 0 };

bool started_s = false;
unsigned int K_s;
sem_t light_s;

void start_s(unsigned int k, size_t MAX_SPAWNS) {
	unsigned int i;
	pthread_t pt;
	if (started_s) {
		fprintf(stderr, "Already started! Aborting.\n");
		return;
	}
	started_s = true;
	K_s = k;

	for (i = 0; i < MAX_TYPE; ++i) {
		state_s.waiting[i] = 0;
		state_s.crossing[i] = 0;
	}

	pthread_create(&pt, NULL, spawner_s, ((void *) MAX_SPAWNS));

	sem_init(&light_s, 0, 1);

	while (true) {
		struct timespec t;
		struct timespec rem;
		t.tv_sec = 1;
		t.tv_nsec = 1000000;
		rem.tv_sec = 0;
		rem.tv_nsec = 0;
		nanosleep(&t, &rem);
	}

	pthread_join(pt, NULL);
}

void try_cross_s(unsigned int type) {
	sem_wait(&light_s);

	if (type >= MAX_TYPE) {
		log_error("Undefined crosser");
		
	} else {
		if (!can_cross(type)) {
			waiting(type);
			sem_post(&light_s);
			// gotta go
			// TODO: cond dótið
			// should now be waiting
			sem_wait(&light_s);
			not_waiting(type);
			cross(type);
			done_crossing_s(type);
		} else {
			sem_post(&light_s);
		}
		cross(type);
	}
}

void * spawner_s(void * argp) {
	struct timespec t;
	struct timespec rem;
	size_t i, MAX_SPAWNS = (size_t) argp;
	char * msg = malloc(MSG_SIZE);

	sprintf(msg, "Spawner initialized: %lu", MAX_SPAWNS);
	log_action(msg);

	t.tv_sec = 0;
	t.tv_nsec = 1000000;
	rem.tv_sec = 0;
	rem.tv_nsec = 0;

	for (i = 0; i < MAX_SPAWNS; i++)
	{
		double x = (double) rand() / (double) RAND_MAX;
		pthread_t pt;
		if (x < 0.5) pthread_create(&pt, NULL, enter_v, NULL);
		else pthread_create(&pt, NULL, enter_p, NULL);
		nanosleep(&t, &rem);
		pthread_detach(pt);
	}

	free(msg);
	return NULL;
}

inline bool can_cross(unsigned int type) {
	unsigned int i, max = 0;
	if (state_s.turn == type || state_s.crossing[type]) {
		return true;
	}

	for (i = 0; i < MAX_TYPE; ++i) { 
		max = (state_s.waiting[i] > max) ? state_s.waiting[i] : max;
		if (type == i) continue;
		if (state_s.crossing[i]) return false;
	}

	if (max == state_s.waiting[type]) {
		state_s.turn = true;
		return true;
	}
	return false;
}

inline void cross(unsigned int type) {
	char *msg = malloc(MSG_SIZE);
	state_s.crossing[type] += 1;
	sprintf(msg, "%u +CROSS", type);
	log_action(msg);
	free(msg);
}

void done_crossing_s(unsigned int type) {
	sem_wait(&light_s);
	char *msg = malloc(MSG_SIZE);
	state_s.crossing[type] -= 1;
	if (state_s.crossing[type] == 0) {
		state_s.turn = type + 1;
		if (state_s.turn >= MAX_TYPE) {
			state_s.turn = 0;
		}
	}
	sprintf(msg, "%u -CROSS", type);
	log_action(msg);
	free(msg);
	sem_post(&light_s);
}

inline void waiting(unsigned int type) {
	state_s.waiting[type] += 1;
}

inline void not_waiting(unsigned int type) {
	unsigned int val;
	val = atomic_load(&(state_s.waiting[type]));
	atomic_store(&(state_s.waiting[type]), val);
}


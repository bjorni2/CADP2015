#include "simple.h"

struct simple state_s = { .turn = -1 };

bool started_s = false;
unsigned int K_s;
sem_t light_s;
sem_t lord_s;
sem_t turn[MAX_TYPE];

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

	sem_init(&light_s, 0, 1);
	sem_init(&turn[PEDESTRIAN], 0, 0);
	sem_init(&turn[VEHICLE], 0, 1);
	state_s.turn = VEHICLE;
	pthread_create(&pt, NULL, spawner_s, ((void *) MAX_SPAWNS));

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
		sem_post(&light_s);
		return;
	} else if (can_cross_s(type)) { // my turn
		sem_wait(&turn[type]);
		cross(type); // start crossing
		sem_post(&turn[type]);
		sem_post(&light_s); // others can start crossing
	} else { // not my turn
		bool lord = false;
		waiting(type);
		if (state_s.waiting[type] == 1) {
			lord = true;
		}
		sem_post(&light_s);

		if (lord) {
			char * msg;
			// set counter
			sem_wait(&lord_s);
			sem_wait(&light_s);
			msg = malloc(MSG_SIZE);
			sprintf(msg, "Lord: %u", type);
			log_action(msg);
			state_s.timeout = true;
			state_s.k = K_s;
			free(msg);
			sem_post(&light_s);
			// cross
			sem_wait(&turn[type]);
			not_waiting(type);
			cross(type);
			sem_post(&turn[type]);
			sem_post(&lord_s);
		} else {
			sem_wait(&turn[type]);
			not_waiting(type);
			cross(type);
			sem_post(&turn[type]);
		}

		return;
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

inline bool can_cross_s(unsigned int type) {
	unsigned int i;
	bool val;
	log_sem(type);
	for (i = 0; i < MAX_TYPE; ++i) {
		if (i == type) continue;
		if (state_s.crossing[type]) {
			return false;
		}
	}

	val = (state_s.turn == type);

	if (val && state_s.timeout) {
		unsigned int k = state_s.k;
		if (k == 0) {
			return false;
		}
		state_s.k = k - 1;
		return true;
	}

	return val;
}

inline void cross(unsigned int type) {
	char *msg = malloc(MSG_SIZE);
	state_s.crossing[type] += 1;
	sprintf(msg, "%u +CROSS [%u in crossing]", type, state_s.crossing[type]);
	log_actionl(msg, 0);
	free(msg);
}

void done_crossing_s(unsigned int type) {
	sem_wait(&light_s);
	char *msg = malloc(MSG_SIZE);
	state_s.crossing[type] -= 1;
	sprintf(msg, "%u -CROSS [%u in crossing]", type, state_s.crossing[type]);
	log_actionl(msg, 0);
	if (state_s.turn != type && state_s.crossing[type] == 0 && state_s.k == 0 && state_s.timeout) {
		unsigned int i;
		for (i = type ; i < MAX_TYPE; ++i) {
			if (state_s.waiting[i]) {
				state_s.turn = i;
				break;
			}
		}
		if (state_s.turn == type) {
			for (i = 0 ; i < type; ++i) {
				if (state_s.waiting[i]) {
					state_s.turn = i;
					break;
				}
			}
		}
		if (state_s.turn != type) {
			state_s.timeout = false;
			state_s.k = K_s;
			sem_wait(&turn[type]);
			sem_post(&turn[state_s.turn]);
		} // else do nothing
	}


	free(msg);
	sem_post(&light_s);
}

inline void waiting(unsigned int type) {
	char *msg = malloc(MSG_SIZE);
	state_s.waiting[type] += 1;
	sprintf(msg, "%u +wait [%u waiting]", type, state_s.waiting[type]);
	log_actionl(msg, 2);
	free(msg);
}

inline void not_waiting(unsigned int type) {
	char *msg = malloc(MSG_SIZE);
	unsigned int val;
	sem_wait(&light_s);
	val = atomic_load(&(state_s.waiting[type]));
	atomic_store(&(state_s.waiting[type]), val);
	sprintf(msg, "%u -wait [%u waiting]", type, state_s.waiting[type]);
	log_actionl(msg, 2);
	free(msg);
	sem_post(&light_s);
}

void log_sem(unsigned int type) {
	char *msg = malloc(MSG_SIZE);
	int s0, s1;
	if (!(sem_getvalue(&turn[0], &s0) && sem_getvalue(&turn[1], &s1))) {
		sprintf(msg, "SEM: [0: %d, 1: %d] type: %u", s0, s1, type);
	} else {
		sprintf(msg, "Something went wrong in sem_value!");
	}
	log_action(msg);
	free(msg);
}

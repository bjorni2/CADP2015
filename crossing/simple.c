#include "simple.h"

struct simple state_s = { .turn = -1 };

bool started_s = false;
unsigned int K_s;
sem_t light_s;
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
	sem_init(&turn[VEHICLE], 0, 0);
	state_s.turn = VEHICLE;
	pthread_create(&pt, NULL, spawner_s, ((void *) MAX_SPAWNS));

	while (true) {
		bool stop = true;
		milli_sleep(2);	
		log_sem(-1);
		for (i = 0; i < MAX_TYPE; ++i) {
			if (atomic_load(&(state_s.waiting[i]))) stop = false;
			if (atomic_load(&(state_s.crossing[i]))) stop = false;
		}
		if (stop) {
			break;
		}
	}

	pthread_join(pt, NULL);
}

void try_cross_s(unsigned int type) {
	if (type >= MAX_TYPE) {
		log_error("Undefined crosser");
		return;
	}

	sem_wait(&light_s);
	if (state_s.crossing[!type] != 0 || state_s.k[!type] == 0) {
		waiting(type);
		if(state_s.waiting[type] == 1){
			state_s.k[type] = K_s;
		}
		sem_post(&light_s);
		sem_wait(&turn[type]);
	}
	sate_s.k[!type]--;
	inc_cross_s(type);
	signal_s();
	rand_sleep(10);
	sem_wait(&light_s);
	done_crossing_s(type);
	signal_s();
}

// PRECONDITION: we have one semaphore
void signal_s() {
	if (state_s.crossing[PEDESTRIAN] == 0 && state_s.waiting[VEHICLE] > 0) {
		not_waiting(VEHICLE);
		sem_post(&turn[VEHICLE]);
	} else if (state_s.crossing[VEHICLE] == 0 && state_s.waiting[PEDESTRIAN] > 0) {
		not_waiting(PEDESTRIAN);
		sem_post(&turn[PEDESTRIAN]);
	} else {
		sem_post(&light_s);
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
	t.tv_nsec = 999999;
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

inline void inc_cross_s(unsigned int type) {
	char *msg = malloc(MSG_SIZE);
	state_s.crossing[type] += 1;
	sprintf(msg, "%u +CROSS [%u in crossing]", type, state_s.crossing[type]);
	log_actionl(msg, 0);
	free(msg);
}

inline void done_crossing_s(unsigned int type) {
	char *msg = malloc(MSG_SIZE);
	state_s.crossing[type] -= 1;
	sprintf(msg, "%u -CROSS [%u in crossing]", type, state_s.crossing[type]);
	log_actionl(msg, 0);
	free(msg);
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
	state_s.waiting[type] -= 1;
	sprintf(msg, "%u -wait [%u waiting]", type, state_s.waiting[type]);
	log_actionl(msg, 2);
	free(msg);
}

void log_sem(unsigned int type) {
	char *msg = malloc(MSG_SIZE);
	int s0, s1;
	long t = (long) ((int) type);
	if (!(sem_getvalue(&turn[0], &s0) && sem_getvalue(&turn[1], &s1))) {
		sprintf(msg, "SEM: [0: %d, 1: %d] type: %ld", s0, s1, t);
	} else {
		sprintf(msg, "Something went wrong in sem_value!");
	}
	log_action(msg);
	free(msg);
}

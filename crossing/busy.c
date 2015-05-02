#include "busy.h"

bool started_b;
unsigned int K_b;
sem_t light_b;
// TODO: these semaphores!! Also change in busy.h and in start_b
sem_t turn_v[MAX_TYPE];
sem_t turn_h[MAX_TYPE];

void start_b(unsigned int k, size_t MAX_SPAWNS) {
	unsigned int i;
	pthread_t pt;
	if (started_b) {
		fprintf(stderr, "Already started! Aborting.\n");
		return;
	}
	started_b = true;
	K_b = k;

	for (i = 0; i < MAX_TYPE; ++i) {
		state_b.crossing_n[i] = 0;
		state_b.crossing_e[i] = 0;
		state_b.waiting_n[i] = 0;
		state_b.waiting_e[i] = 0;
		// TODO: others!
	}

	sem_init(&light_b, 0, 1);
	sem_init(&turn_v[PEDESTRIAN], 0, 0);
	sem_init(&turn_v[VEHICLE], 0, 0);
	sem_init(&turn_h[PEDESTRIAN], 0, 0);
	sem_init(&turn_h[VEHICLE], 0, 0);

	pthread_create(&pt, NULL, spawner_c, ((void *) MAX_SPAWNS));

	while (true) {
		bool stop = true;
		milli_sleep(2);	
		for (i = 0; i < MAX_TYPE; ++i) {
			if (atomic_load(&(state_b.waiting_n[i]))) stop = false;
			if (atomic_load(&(state_b.crossing_n[i]))) stop = false;
			if (atomic_load(&(state_b.waiting_e[i]))) stop = false;
			if (atomic_load(&(state_b.crossing_e[i]))) stop = false;
		}
		if (stop) {
			break;
		}
	}

	pthread_join(pt, NULL);
}

void * spawner_b(void * argp) {
	size_t i, MAX_SPAWNS = (size_t) argp;
	unsigned int v=0,p=0;
	char * msg = malloc(MSG_SIZE);
	sprintf(msg, "Spawner initialized: %lu", MAX_SPAWNS);
	log_action(msg);

	for (i = 0; i < MAX_SPAWNS; i++)
	{
		double x = (double) rand() / (double) RAND_MAX;
//		pthread_t pt;
		if (x < 0.5) {
//			pthread_create(&pt, NULL, enter_v_c, NULL);	
			v++;
		} else { 
//			pthread_create(&pt, NULL, enter_p_c, NULL);
			p++;
		}
		milli_sleep(10);
//		pthread_detach(pt);
	}

	sprintf(msg, "Spawned %u vehicles and %u pedestrians.", v, p);
	log_action(msg);

	free(msg);
	return NULL;
}


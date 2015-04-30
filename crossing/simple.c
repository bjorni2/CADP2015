#include "simple.h"

bool started_s = false;
unsigned int K_s;
sem_t light_s;

void start_s(unsigned int k, size_t MAX_SPAWNS) {
	pthread_t pt;
	if (started_s) {
		fprintf(stderr, "Already started! Aborting.\n");
		return;
	}
	started_s = true;
	K_s = k;

	pthread_create(&pt, NULL, spawner_s, ((void *) MAX_SPAWNS));

	sem_init(&light_s, 0, 1);

	pthread_join(pt, NULL);
}

void try_cross_s(unsigned int type) {
	sem_wait(&light_s);

	if (type == PEDESTRIAN) {
		
	} else if (type == VEHICLE) {

	} else {
		log_error("Undefined crosser");
	}

	sem_post(&light_s);
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

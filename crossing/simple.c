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
	printf("hi\n");
	started_s = true;
	K_s = k;

	pthread_create(&pt, NULL, spawner_s, ((void *) MAX_SPAWNS));

	sem_init(&light_s, 0, 1);

	pthread_join(pt, NULL);
}

void try_cross(unsigned int type) {
	sem_wait(&light_s);

	if (type == PEDESTRIAN) {
		
	} else if (type == VEHICLE) {

	} else {
		log_error("Undefined crosser");
	}

	sem_post(&light_s);
}

void * spawner_s(void * argp) {
	size_t MAX_SPAWNS = (size_t) argp;
	char * msg = malloc(MSG_SIZE);

	sprintf(msg, "Spawner initialized: %lu", MAX_SPAWNS);
	log_action(msg);

	// TODO: Spawn some stuff randomnly


	free(msg);
	return NULL;
}

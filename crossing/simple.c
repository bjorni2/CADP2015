#include "simple.h"

bool started_s = false;
unsigned int K_s;

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

	// TODO: controller stuff
	
	pthread_join(pt, NULL);
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

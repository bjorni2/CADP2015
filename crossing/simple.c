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

	struct timespec t;
	t.tv_sec = 0;
	t.tv_nsec = 1000000;
	struct timespec rem;
	rem.tv_sec = 0;
	rem.tv_nsec = 0;

	// TODO: Spawn some stuff randomnly
    for (int i = 0; i < MAX_SPAWN; i++)
    {
        double x = (double) rand() / (double) RAND_MAX;
        if (x < 0.5) pthread_create(NULL, NULL, enter_v, NULL);
        else pthread_create(NULL, NULL, enter_p, NULL);
        nanosleep(&t, &rem);
    }

	free(msg);
	return NULL;
}

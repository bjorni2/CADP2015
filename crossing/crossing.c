#include "crossing.h"

void start_b(unsigned int k, size_t MAX_SPAWNS) {

}

void * spawner_b(void * argp) {
	size_t i, MAX_SPAWNS = (size_t) argp;
	sprintf(msg, "Spawner initialized: %lu", MAX_SPAWNS);
	log_action(msg);

	for (i = 0; i < MAX_SPAWNS; i++)
	{
		double x = (double) rand() / (double) RAND_MAX;
		pthread_t pt;
		if (x < 0.5) {
			pthread_create(&pt, NULL, enter_v_c, NULL);	
		} else { 
			pthread_create(&pt, NULL, enter_p_c, NULL);
		}
		milli_sleep(1000);
		pthread_detach(pt);
	}

	free(msg);
	return NULL;
}


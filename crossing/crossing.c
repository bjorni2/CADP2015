#include "crossing.h"

struct crossing state_c;

bool started_s = false;
unsigned int K_s;
sem_t light_s;
sem_t turn_v[MAX_TYPE];
sem_t turn_h[MAX_TYPE];

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

void try_cross_ped(unsigned int dir){
	sem_wait(&light_s);
	if(dir == VERTICAL && c.crossing_e[VEHICLE] != 0){
		waiting(PEDESTRIAN, dir);
		sem_post(&light_s);
		sem_wait(&turn_v[PEDESTRIAN]);
	}
	else if(dir == HORIZONTAL && c.crossing_n[VEHICLE] != 0){
		waiting(PEDESTRIAN, dir);
		sem_post(&light_s);
		sem_wait(&turn_h[PEDESTRIAN]);
	}
	
	
}
void try_cross_veh(unsigned int dir);

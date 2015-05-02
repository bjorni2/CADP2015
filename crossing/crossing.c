#include "crossing.h"

struct crossing state_c;

bool started_c = false;
unsigned int K_c;
sem_t light_c;
sem_t turn_v[MAX_TYPE];
sem_t turn_h[MAX_TYPE];

void start_c(unsigned int k, size_t MAX_SPAWNS) {
	unsigned int i;
	pthread_t pt;
	if (started_c) {
		fprintf(stderr, "Already started! Aborting.\n");
		return;
	}
	started_c = true;
	K_c = k;

	for (i = 0; i < MAX_TYPE; ++i) {
		state_c.crossing_v[i] = 0;
		state_c.crossing_h[i] = 0;
		state_c.waiting_v[i] = 0;
		state_c.waiting_h[i] = 0;
		state_c.k[i] = 0;
		state_c.last[i] = true;
	}
	state_c.last_pv = true;
	
	sem_init(&light_c, 0, 1);
	sem_init(&turn_v[PEDESTRIAN], 0, 0);
	sem_init(&turn_v[VEHICLE], 0, 0);
	sem_init(&turn_h[PEDESTRIAN], 0, 0);
	sem_init(&turn_h[VEHICLE], 0, 0);

	pthread_create(&pt, NULL, spawner_c, ((void *) MAX_SPAWNS));

	while (true) {
		bool stop = true;
		milli_sleep(2);	
		for (i = 0; i < MAX_TYPE; ++i) {
			if (atomic_load(&(state_c.waiting_v[i]))) stop = false;
			if (atomic_load(&(state_c.crossing_v[i]))) stop = false;
			if (atomic_load(&(state_c.waiting_h[i]))) stop = false;
			if (atomic_load(&(state_c.crossing_h[i]))) stop = false;
		}
		if (stop) {
			break;
		}
	}

	pthread_join(pt, NULL);
}

void * spawner_c(void * argp) {
	size_t i, MAX_SPAWNS = (size_t) argp;
	unsigned int v=0,p=0;
	char * msg = malloc(MSG_SIZE);
	sprintf(msg, "Spawner initialized: %lu", MAX_SPAWNS);
	log_action(msg);

	for (i = 0; i < MAX_SPAWNS; i++)
	{
		double x = (double) rand() / (double) RAND_MAX;
		pthread_t pt;
		if (x < 0.5) {
			pthread_create(&pt, NULL, enter_v_c, NULL);	
			v++;
		} else { 
			pthread_create(&pt, NULL, enter_p_c, NULL);
			p++;
		}
		milli_sleep(10);
		pthread_detach(pt);
	}

	sprintf(msg, "Spawned %u vehicles and %u pedestrians.", v, p);
	log_action(msg);

	free(msg);
	return NULL;
}

void try_cross_ped(unsigned int dir){
	sem_wait(&light_c);
	if(dir == VERTICAL && state_c.crossing_h[VEHICLE] != 0){
		waiting_c(PEDESTRIAN, dir);
		sem_post(&light_c);
		sem_wait(&turn_v[PEDESTRIAN]);
	}
	else if(dir == HORIZONTAL && state_c.crossing_v[VEHICLE] != 0){
		waiting_c(PEDESTRIAN, dir);
		sem_post(&light_c);
		sem_wait(&turn_h[PEDESTRIAN]);
	}
	
	inc_cross_c(PEDESTRIAN, dir);
	signal_c();
	//crossing, sleep?
	rand_sleep(900);
	sem_wait(&light_c);
	done_inc_cross_c(PEDESTRIAN, dir);
	signal_c();
}

void try_cross_veh(unsigned int dir){
	sem_wait(&light_c);
	if(dir == VERTICAL && (state_c.crossing_h[VEHICLE] != 0 || state_c.crossing_h[PEDESTRIAN] != 0)){
		waiting_c(VEHICLE, dir);
		sem_post(&light_c);
		sem_wait(&turn_v[VEHICLE]);
	}
	else if(dir == HORIZONTAL && (state_c.crossing_v[VEHICLE] != 0 || state_c.crossing_v[PEDESTRIAN] != 0)){
		waiting_c(VEHICLE, dir);
		sem_post(&light_c);
		sem_wait(&turn_h[VEHICLE]);
	}
	
	inc_cross_c(VEHICLE, dir);
	signal_c();
	//crossing, sleep?
	rand_sleep(500);
	sem_wait(&light_c);
	done_inc_cross_c(VEHICLE, dir);
	signal_c();
}

void signal_c(){
	if(state_c.waiting_v[VEHICLE] > 0 && state_c.crossing_h[VEHICLE] == 0 && state_c.crossing_h[PEDESTRIAN] == 0){
		if(state_c.waiting_v[PEDESTRIAN] > 0){
			if(state_c.last[VERTICAL] == true){
				state_c.last[VERTICAL] = false;
				not_waiting_c(VEHICLE, VERTICAL);
				sem_post(&turn_v[VEHICLE]);
			}
			else{
				state_c.last[VERTICAL] = true;
				not_waiting_c(PEDESTRIAN, VERTICAL);
				sem_post(&turn_v[PEDESTRIAN]);
			}
		}
		else{
			not_waiting_c(VEHICLE, VERTICAL);
			sem_post(&turn_v[VEHICLE]);
		}
	}
	else if(state_c.waiting_h[VEHICLE] > 0 && state_c.crossing_v[VEHICLE] == 0 && state_c.crossing_v[PEDESTRIAN] == 0){
		if(state_c.waiting_h[PEDESTRIAN] > 0){
			if(state_c.last[HORIZONTAL] == true){
				state_c.last[HORIZONTAL] = false;
				not_waiting_c(VEHICLE, HORIZONTAL);
				sem_post(&turn_h[VEHICLE]);
			}
			else{
				state_c.last[HORIZONTAL] = true;
				not_waiting_c(PEDESTRIAN, HORIZONTAL);
				sem_post(&turn_h[PEDESTRIAN]);
			}
		}
		else{
			not_waiting_c(VEHICLE, HORIZONTAL);
			sem_post(&turn_h[VEHICLE]);
		}
	}
	else if(state_c.waiting_h[PEDESTRIAN] > 0 && state_c.crossing_v[VEHICLE] == 0){
		if(state_c.waiting_v[PEDESTRIAN] > 0){
			if(state_c.last_pv == true){
				state_c.last_pv = false;
				not_waiting_c(PEDESTRIAN, HORIZONTAL);
				sem_post(&turn_h[PEDESTRIAN]);
			}
			else{
				state_c.last_pv = true;
				not_waiting_c(PEDESTRIAN, VERTICAL);
				sem_post(&turn_v[PEDESTRIAN]);
			}
		}
		else{
			not_waiting_c(PEDESTRIAN, HORIZONTAL);
			sem_post(&turn_h[PEDESTRIAN]);
		}
	}
	else if(state_c.waiting_v[PEDESTRIAN] > 0 && state_c.crossing_h[VEHICLE] == 0){
		not_waiting_c(PEDESTRIAN, VERTICAL);
		sem_post(&turn_v[PEDESTRIAN]);
	}
	else{
		sem_post(&light_c);
	}
}

/*void signal_c(){
	if(state_c.waiting_v[VEHICLE] > 0 && state_c.crossing_h[VEHICLE] == 0 && state_c.crossing_h[PEDESTRIAN] == 0){
		not_waiting_c(VEHICLE, VERTICAL);
		sem_post(&turn_v[VEHICLE]);
	}
	else if(state_c.waiting_v[PEDESTRIAN] > 0 && state_c.crossing_h[VEHICLE] == 0){
		not_waiting_c(PEDESTRIAN, VERTICAL);
		sem_post(&turn_v[PEDESTRIAN]);
	}
	else if(state_c.waiting_h[VEHICLE] > 0 && state_c.crossing_v[VEHICLE] == 0 && state_c.crossing_v[PEDESTRIAN] == 0){
		not_waiting_c(VEHICLE, HORIZONTAL);
		sem_post(&turn_h[VEHICLE]);
	}
	else if(state_c.waiting_h[PEDESTRIAN] > 0 && state_c.crossing_v[VEHICLE] == 0){
		not_waiting_c(PEDESTRIAN, HORIZONTAL);
		sem_post(&turn_h[PEDESTRIAN]);
	}
	else{
		sem_post(&light_c);
	}
}*/

inline void inc_cross_c(unsigned int type, unsigned int dir) {
	char *msg = malloc(MSG_SIZE);
	unsigned int crossers = 0;
	switch (dir) {
		case HORIZONTAL:
			state_c.crossing_h[type] += 1;
			crossers = state_c.crossing_h[type];
			break;
		case VERTICAL:
			state_c.crossing_v[type] += 1;
			crossers = state_c.crossing_v[type];
			break;
	}
	sprintf(msg, "%u, %u +CROSS [%u crossing]", type, dir, crossers);
	log_actionl(msg, 0);
	free(msg);
}

inline void done_inc_cross_c(unsigned int type, unsigned int dir) { 
	char *msg = malloc(MSG_SIZE);
	unsigned int crossers = 0;
	switch (dir) {
		case HORIZONTAL:
			state_c.crossing_h[type] -= 1;
			crossers = state_c.crossing_h[type];
			break;
		case VERTICAL:
			state_c.crossing_v[type] -= 1;
			crossers = state_c.crossing_h[type];
			break;
	}
	sprintf(msg, "%u, %u -CROSS [%u crossing]", type, dir, crossers);
	log_actionl(msg, 0);
	free(msg);
}

inline void waiting_c(unsigned int type, unsigned int dir) {
	char *msg = malloc(MSG_SIZE);
	unsigned int waiters = 0;
	switch (dir) {
		case HORIZONTAL:
			state_c.waiting_h[type] += 1;
			waiters = state_c.waiting_h[type];
			break;
		case VERTICAL:
			state_c.waiting_v[type] += 1;
			waiters = state_c.waiting_v[type];
			break;
	}
	sprintf(msg, "%u, %u +wait [%u waiting]", type, dir, waiters);
	log_actionl(msg, 2);
	free(msg);
}

inline void not_waiting_c(unsigned int type, unsigned int dir) {
	char *msg = malloc(MSG_SIZE);
	unsigned int waiters = 0;
	switch (dir) {
		case HORIZONTAL:
			state_c.waiting_h[type] -= 1;
			waiters = state_c.waiting_h[type];
			break;
		case VERTICAL:
			state_c.waiting_v[type] -= 1;
			waiters = state_c.waiting_v[type];
			break;
	}
	sprintf(msg, "%u, %u -wait [%u waiting]", type, dir, waiters);
	log_actionl(msg, 2);
	free(msg);
}


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
//		log_sem(-1);
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
	char * msg = malloc(MSG_SIZE);
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
		milli_sleep(10);
		pthread_detach(pt);
	}

	free(msg);
	return NULL;
}

void try_cross_ped(unsigned int dir){
	sem_wait(&light_c);
	if((dir == VERTICAL && state_c.crossing_h[VEHICLE] != 0) || state_c.k[HORIZONTAL] == 0){
		waiting_c(PEDESTRIAN, dir);
		if(state_c.waiting_v[VEHICLE] == 0 && state_c.waiting_v[PEDESTRIAN] == 1){
			state_c.k[VERTICAL] = K_c;
		}
		sem_post(&light_c);
		sem_wait(&turn_v[PEDESTRIAN]);
	}
	else if((dir == HORIZONTAL && state_c.crossing_v[VEHICLE] != 0) || state_c.k[VERTICAL] == 0){
		waiting_c(PEDESTRIAN, dir);
		if(state_c.waiting_h[VEHICLE] == 0 && state_c.waiting_h[PEDESTRIAN] == 1){
			state_c.k[HORIZONTAL] = K_c;
		}
		sem_post(&light_c);
		sem_wait(&turn_h[PEDESTRIAN]);
	}
	
	if(dir == VERTICAL && state_c.waiting_h[VEHICLE] > 0){
		state_c.k[HORIZONTAL]--;
	}
	else if(dir == HORIZONTAL && state_c.waiting_v[VEHICLE] > 0){
		state_c.k[VERTICAL]--;
	}
	
	inc_cross_c(PEDESTRIAN, dir);
	
	if(dir == VERTICAL && state_c.waiting_h[VEHICLE] > 0 && state_c.k[HORIZONTAL] == 0){ // Ef k er 0 megum við bara opna light_s
		sem_post(&light_s);
	}
	else if(dir == HORIZONTAL && state_c.waiting_v[VEHICLE] > 0 && state_c.k[VERTICAL] == 0){
		sem_post(&light_s);
	}
	else{
		signal_c();
	}
	//crossing, sleep?
	rand_sleep(900);
	sem_wait(&light_c);
	done_inc_cross_c(PEDESTRIAN, dir);
	
	if(dir == VERTICAL && state_c.crossing_v[PEDESTRIAN] == 0 && state_c.crossing_v[VEHICLE] == 0 && state_c.waiting_h[VEHICLE] > 0 /*&& state_s.k[!type] == 0*/){
		state_c.k[HORIZONTAL] = K_c;
		not_waiting_c(VEHICLE, HORIZONTAL);
		sem_post(&turn_h[VEHICLE]);
	}
	else if(dir == HORIZONTAL && state_c.crossing_h[PEDESTRIAN] == 0 && state_c.crossing_h[VEHICLE] == 0 && state_c.waiting_v[VEHICLE] > 0 /*&& state_s.k[!type] == 0*/){
		state_c.k[VERTICAL] = K_c;
		not_waiting_c(VEHICLE, VERTICAL);
		sem_post(&turn_v[VEHICLE]);
	}
	else{
		signal_c();
	}
}


void try_cross_veh(unsigned int dir){
	sem_wait(&light_c);
	if((dir == VERTICAL && (state_c.crossing_h[VEHICLE] != 0 || state_c.crossing_h[PEDESTRIAN] != 0)) || state_c.k[HORIZONTAL] == 0){
		waiting_c(VEHICLE, dir);
		if(state_c.waiting_v[PEDESTRIAN] == 0 && state_c.waiting_v[VEHICLE] == 1){
			state_c.k[VERTICAL] = K_c;
		}
		sem_post(&light_c);
		sem_wait(&turn_v[VEHICLE]);
	}
	else if((dir == HORIZONTAL && (state_c.crossing_v[VEHICLE] != 0 || state_c.crossing_v[PEDESTRIAN] != 0)) || state_c.k[VERTICAL] == 0){
		waiting_c(VEHICLE, dir);
		if(state_c.waiting_h[PEDESTRIAN] == 0 && state_c.waiting_h[VEHICLE] == 1){
			state_c.k[HORIZONTAL] = K_c;
		}
		sem_post(&light_c);
		sem_wait(&turn_h[VEHICLE]);
	}
	
	if(dir == VERTICAL && (state_c.waiting_h[VEHICLE] > 0 || state_c.waiting_h[PEDESTRIAN])){
		state_c.k[HORIZONTAL]--;
	}
	else if(dir == HORIZONTAL && (state_c.waiting_v[VEHICLE] > 0 || state_c.waiting_v[PEDESTRIAN] > 0)){
		state_c.k[VERTICAL]--;
	}
	
	
	inc_cross_c(VEHICLE, dir);
	
	
	if(dir == VERTICAL && (state_c.waiting_h[VEHICLE] > 0 || state_c.waiting_h[PEDESTRIAN]) && state_c.k[HORIZONTAL] == 0){ // Ef k er 0 megum við bara opna light_s
		sem_post(&light_s);
	}
	else if(dir == HORIZONTAL && (state_c.waiting_v[VEHICLE] > 0 || state_c.waiting_v[PEDESTRIAN] > 0) && state_c.k[VERTICAL] == 0){
		sem_post(&light_s);
	}
	else{
		signal_c();
	}
	//crossing, sleep?
	rand_sleep(500);
	sem_wait(&light_c);
	done_inc_cross_c(VEHICLE, dir);
	
	if(dir == VERTICAL && state_c.crossing_v[PEDESTRIAN] == 0 && state_c.crossing_v[VEHICLE] == 0 && state_c.waiting_h[VEHICLE] > 0){
		state_c.k[HORIZONTAL] = K_c;
		not_waiting_c(VEHICLE, HORIZONTAL);
		sem_post(&turn_h[VEHICLE]);
	}
	else if(dir == VERTICAL && state_c.crossing_v[VEHICLE] == 0 && state_c.waiting_h[PEDESTRIAN] > 0 && state_c.waiting_h[VEHICLE] == 0){
		state_c.k[HORIZONTAL] = K_c;
		not_waiting_c(PEDESTRIAN, HORIZONTAL);
		sem_post(&turn_h[PEDESTRIAN]);
	}
	else if(dir == HORIZONTAL && state_c.crossing_h[PEDESTRIAN] == 0 && state_c.crossing_h[VEHICLE] == 0 && state_c.waiting_v[VEHICLE] > 0){
		state_c.k[VERTICAL] = K_c;
		not_waiting_c(VEHICLE, VERTICAL);
		sem_post(&turn_v[VEHICLE]);
	}
	else if(dir == HORIZONTAL && state_c.crossing_h[VEHICLE] == 0 && state_c.waiting_v[PEDESTRIAN] > 0 && state_c.waiting_v[VEHICLE] == 0){
		state_c.k[VERTICAL] = K_c;
		not_waiting_c(PEDESTRIAN, VERTICAL);
		sem_post(&turn_h[VERTICAL]);
	}
	else{
		signal_c();
	}
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

// TODO: log
inline void inc_cross_c(unsigned int type, unsigned int dir) {
	switch (dir) {
		case HORIZONTAL:
			state_c.crossing_h[type] += 1;
			break;
		case VERTICAL:
			state_c.crossing_v[type] += 1;
			break;
	}
}

inline void done_inc_cross_c(unsigned int type, unsigned int dir) { 
	switch (dir) {
		case HORIZONTAL:
			state_c.crossing_h[type] -= 1;
			break;
		case VERTICAL:
			state_c.crossing_v[type] -= 1;
			break;
	}
}

inline void waiting_c(unsigned int type, unsigned int dir) {
	switch (dir) {
		case HORIZONTAL:
			state_c.waiting_h[type] += 1;
			break;
		case VERTICAL:
			state_c.waiting_v[type] += 1;
			break;
	}
}

inline void not_waiting_c(unsigned int type, unsigned int dir) {
	switch (dir) {
		case HORIZONTAL:
			state_c.waiting_h[type] -= 1;
			break;
		case VERTICAL:
			state_c.waiting_v[type] -= 1;
			break;
	}
}


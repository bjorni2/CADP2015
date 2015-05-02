#include "crossing.h"

struct crossing state_c;

bool started_c = false;
unsigned int K_c;
sem_t light_c;
sem_t turn_c[2][MAX_TYPE];

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
		state_c.crossing[VERTICAL][i] = 0;
		state_c.crossing[HORIZONTAL][i] = 0;
		state_c.waiting[VERTICAL][i] = 0;
		state_c.waiting[HORIZONTAL][i] = 0;
		state_c.k[i] = 1;
		state_c.last[i] = true;
	}
	state_c.last_pv = true;
	
	sem_init(&light_c, 0, 1);
	sem_init(&turn_c[VERTICAL][PEDESTRIAN], 0, 0);
	sem_init(&turn_c[VERTICAL][VEHICLE], 0, 0);
	sem_init(&turn_c[HORIZONTAL][PEDESTRIAN], 0, 0);
	sem_init(&turn_c[HORIZONTAL][VEHICLE], 0, 0);

	pthread_create(&pt, NULL, spawner_c, ((void *) MAX_SPAWNS));

	while (true) {
		bool stop = true;
		milli_sleep(2);	
		for (i = 0; i < MAX_TYPE; ++i) {
			if (atomic_load(&(state_c.waiting[VERTICAL][i]))) stop = false;
			if (atomic_load(&(state_c.crossing[VERTICAL][i]))) stop = false;
			if (atomic_load(&(state_c.waiting[HORIZONTAL][i]))) stop = false;
			if (atomic_load(&(state_c.crossing[HORIZONTAL][i]))) stop = false;
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
	log_sem_c();
	sem_wait(&light_c);
	if ((state_c.crossing[!dir][VEHICLE] != 0 || state_c.k[!dir] == 0)) {
		waiting_c(PEDESTRIAN, dir);
		if(state_c.waiting[dir][VEHICLE] == 0 && state_c.waiting[dir][PEDESTRIAN] == 1){
			state_c.k[dir] = K_c;
		}
		sem_post(&light_c);
		sem_wait(&turn_c[dir][PEDESTRIAN]);
	}
	
	if(state_c.waiting[!dir][VEHICLE] > 0){
		state_c.k[!dir]--;
	}
	
	inc_cross_c(PEDESTRIAN, dir);
	
	if(state_c.waiting[!dir][VEHICLE] > 0 && state_c.k[!dir] == 0) {
		sem_post(&light_s);
	} else {
		signal_c();
	}
	//crossing, sleep?
	rand_sleep(900);
	sem_wait(&light_c);
	done_inc_cross_c(PEDESTRIAN, dir);
	
	if(state_c.crossing[dir][PEDESTRIAN] == 0 && state_c.crossing[dir][VEHICLE] == 0 && state_c.waiting[!dir][VEHICLE] > 0 /*&& state_s.k[!type] == 0*/){
		state_c.k[!dir] = K_c;
		not_waiting_c(VEHICLE, !dir);
		sem_post(&turn_c[!dir][VEHICLE]);
	} else{
		signal_c();
	}
}

void try_cross_veh(unsigned int dir){
	log_sem_c();
	sem_wait(&light_c);
	if ((state_c.crossing[!dir][VEHICLE] != 0 || state_c.crossing[!dir][PEDESTRIAN] != 0) || state_c.k[!dir] == 0) {
		waiting_c(VEHICLE, dir);
		if(state_c.waiting[dir][PEDESTRIAN] == 0 && state_c.waiting[dir][VEHICLE] == 1){
			state_c.k[dir] = K_c;
		}
		sem_post(&light_c);
		sem_wait(&turn_c[dir][VEHICLE]);
	}
	
	if(state_c.waiting[!dir][VEHICLE] > 0 || state_c.waiting[!dir][PEDESTRIAN]) {
		state_c.k[!dir]--;
	}
	
	inc_cross_c(VEHICLE, dir);
	
	if ((state_c.waiting[!dir][VEHICLE] > 0 || state_c.waiting[!dir][PEDESTRIAN]) && state_c.k[!dir] == 0){ // Ef k er 0 megum við bara opna light_s
		sem_post(&light_s);
	}
	else{
		signal_c();
	}
	//crossing, sleep?
	rand_sleep(500);
	sem_wait(&light_c);
	done_inc_cross_c(VEHICLE, dir);
	
	if(state_c.crossing[dir][PEDESTRIAN] == 0 && state_c.crossing[dir][VEHICLE] == 0 && state_c.waiting[!dir][VEHICLE] > 0){
			state_c.k[!dir] = K_c;
			not_waiting_c(VEHICLE, !dir);
			sem_post(&turn_c[!dir][VEHICLE]);
	} else if(state_c.crossing[dir][VEHICLE] == 0 && state_c.waiting[!dir][PEDESTRIAN] > 0 && state_c.waiting[!dir][VEHICLE] == 0){
			state_c.k[!dir] = K_c;
			not_waiting_c(PEDESTRIAN, !dir);
			sem_post(&turn_c[!dir][PEDESTRIAN]);
	} else {
		signal_c();
	}
}

void signal_c(){
	log_sem_c();
	if(state_c.waiting[VERTICAL][VEHICLE] > 0 && state_c.crossing[HORIZONTAL][VEHICLE] == 0 && state_c.crossing[HORIZONTAL][PEDESTRIAN] == 0){
		if(state_c.waiting[VERTICAL][PEDESTRIAN] > 0){
			if(state_c.last[VERTICAL] == true){
				state_c.last[VERTICAL] = false;
				not_waiting_c(VEHICLE, VERTICAL);
				sem_post(&turn_c[VERTICAL][VEHICLE]);
			}
			else{
				state_c.last[VERTICAL] = true;
				not_waiting_c(PEDESTRIAN, VERTICAL);
				sem_post(&turn_c[VERTICAL][PEDESTRIAN]);
			}
		}
		else{
			not_waiting_c(VEHICLE, VERTICAL);
			sem_post(&turn_c[VERTICAL][VEHICLE]);
		}
	}
	else if(state_c.waiting[HORIZONTAL][VEHICLE] > 0 && state_c.crossing[VERTICAL][VEHICLE] == 0 && state_c.crossing[VERTICAL][PEDESTRIAN] == 0){
		if(state_c.waiting[HORIZONTAL][PEDESTRIAN] > 0){
			if(state_c.last[HORIZONTAL] == true){
				state_c.last[HORIZONTAL] = false;
				not_waiting_c(VEHICLE, HORIZONTAL);
				sem_post(&turn_c[HORIZONTAL][VEHICLE]);
			}
			else{
				state_c.last[HORIZONTAL] = true;
				not_waiting_c(PEDESTRIAN, HORIZONTAL);
				sem_post(&turn_c[HORIZONTAL][PEDESTRIAN]);
			}
		}
		else{
			not_waiting_c(VEHICLE, HORIZONTAL);
			sem_post(&turn_c[HORIZONTAL][VEHICLE]);
		}
	}
	else if(state_c.waiting[HORIZONTAL][PEDESTRIAN] > 0 && state_c.crossing[VERTICAL][VEHICLE] == 0){
		if(state_c.waiting[VERTICAL][PEDESTRIAN] > 0){
			if(state_c.last_pv == true){
				state_c.last_pv = false;
				not_waiting_c(PEDESTRIAN, HORIZONTAL);
				sem_post(&turn_c[HORIZONTAL][PEDESTRIAN]);
			}
			else{
				state_c.last_pv = true;
				not_waiting_c(PEDESTRIAN, VERTICAL);
				sem_post(&turn_c[VERTICAL][PEDESTRIAN]);
			}
		}
		else{
			not_waiting_c(PEDESTRIAN, HORIZONTAL);
			sem_post(&turn_c[HORIZONTAL][PEDESTRIAN]);
		}
	}
	else if(state_c.waiting[VERTICAL][PEDESTRIAN] > 0 && state_c.crossing[HORIZONTAL][VEHICLE] == 0){
		not_waiting_c(PEDESTRIAN, VERTICAL);
		sem_post(&turn_c[VERTICAL][PEDESTRIAN]);
	}
	else{
		sem_post(&light_c);
	}
}

inline void inc_cross_c(unsigned int type, unsigned int dir) {
	char *msg = malloc(MSG_SIZE);
	unsigned int crossers = 0;
	switch (dir) {
		case HORIZONTAL:
			state_c.crossing[HORIZONTAL][type] += 1;
			crossers = state_c.crossing[HORIZONTAL][type];
			break;
		case VERTICAL:
			state_c.crossing[VERTICAL][type] += 1;
			crossers = state_c.crossing[VERTICAL][type];
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
			state_c.crossing[HORIZONTAL][type] -= 1;
			crossers = state_c.crossing[HORIZONTAL][type];
			break;
		case VERTICAL:
			state_c.crossing[VERTICAL][type] -= 1;
			crossers = state_c.crossing[HORIZONTAL][type];
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
			state_c.waiting[HORIZONTAL][type] += 1;
			waiters = state_c.waiting[HORIZONTAL][type];
			break;
		case VERTICAL:
			state_c.waiting[VERTICAL][type] += 1;
			waiters = state_c.waiting[VERTICAL][type];
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
			state_c.waiting[HORIZONTAL][type] -= 1;
			waiters = state_c.waiting[HORIZONTAL][type];
			break;
		case VERTICAL:
			state_c.waiting[VERTICAL][type] -= 1;
			waiters = state_c.waiting[VERTICAL][type];
			break;
	}
	sprintf(msg, "%u, %u -wait [%u waiting]", type, dir, waiters);
	log_actionl(msg, 2);
	free(msg);
}

void log_sem_c() {
	char * msg = malloc(MSG_SIZE);
	int l, v0, v1, h0, h1;
	sem_getvalue(&light_c, &l);
	sem_getvalue(&turn_c[VERTICAL][0], &v0);
	sem_getvalue(&turn_c[VERTICAL][1], &v1);
	sem_getvalue(&turn_c[HORIZONTAL][0], &h0);
	sem_getvalue(&turn_c[HORIZONTAL][1], &h1);
	sprintf(msg, "[L, V0, V1, H0, H1]: %d %d %d %d %d", l, v0, v1, h0, h1);
	log_actionl(msg, 4);
//	sprintf(msg, "S: CV0, CV1, CH0, CH1, WV0, WV1, WH0, WH1, KV, KH");
//	log_actionl(msg, 4);
	sprintf(msg, "S: %3u, %3u, %3u, %3u, %3u, %3u, %3u, %3u, %2u, %2u",
			state_c.crossing[VERTICAL][0],
			state_c.crossing[VERTICAL][1],
			state_c.crossing[HORIZONTAL][0],
			state_c.crossing[HORIZONTAL][1],
			state_c.waiting[VERTICAL][0],
			state_c.waiting[VERTICAL][1],
			state_c.waiting[HORIZONTAL][0],
			state_c.waiting[HORIZONTAL][1],
			state_c.k[0],
			state_c.k[1]
			);
	log_actionl(msg, 12);
	free(msg);
}


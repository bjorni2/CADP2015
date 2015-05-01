#include "crossing.h"

struct crossing state_c ;

bool started_s = false;
unsigned int K_s;
sem_t light_s;
sem_t turn_v[MAX_TYPE];
sem_t turn_h[MAX_TYPE];

void start_b(unsigned int k, size_t MAX_SPAWNS) {

}

void try_cross_ped(unsigned int dir){
	sem_wait(&light_s);
	if(dir == VERTICAL && state_c.crossing_e[VEHICLE] != 0){
		waiting_c(PEDESTRIAN, dir);
		sem_post(&light_s);
		sem_wait(&turn_v[PEDESTRIAN]);
	}
	else if(dir == HORIZONTAL && state_c.crossing_n[VEHICLE] != 0){
		waiting_c(PEDESTRIAN, dir);
		sem_post(&light_s);
		sem_wait(&turn_h[PEDESTRIAN]);
	}
	
	crossing_c(PEDESTRIAN, dir);
	signal_c();
	//crossing, sleep?
	sem_wait(&light_s);
	done_crossing_c(PEDESTRIAN, dir);
	signal_c();
}

void try_cross_veh(unsigned int dir){
	sem_wait(&light_s);
	if(dir == VERTICAL && (state_c.crossing_e[VEHICLE] != 0 || state_c.crossing_e[PEDESTRIAN] != 0)){
		waiting_c(VEHICLE, dir);
		sem_post(&light_s);
		sem_wait(&turn_v[VEHICLE]);
	}
	else if(dir == HORIZONTAL && (state_c.crossing_n[VEHICLE] != 0 || state_c.crossing_n[PEDESTRIAN] != 0)){
		waiting_c(VEHICLE, dir);
		sem_post(&light_s);
		sem_wait(&turn_h[VEHICLE]);
	}
	
	crossing_c(VEHICLE, dir);
	signal_c();
	//crossing, sleep?
	sem_wait(&light_s);
	done_crossing_c(VEHICLE, dir);
	signal_c();
}

void signal_c(){
	if(state_c.waiting_n[VEHICLE] > 0 && state_c.crossing_e[VEHICLE] == 0 && state_c.crossing_e[PEDESTRIAN] == 0){
		not_waiting_c(VEHICLE, VERTICAL);
		sem_post(&turn_v[VEHICLE]);
	}
	else if(state_c.waiting_n[PEDESTRIAN] > 0 && state_c.crossing_e[VEHICLE] == 0){
		not_waiting_c(PEDESTRIAN, VERTICAL);
		sem_post(&turn_v[PEDESTRIAN]);
	}
	else if(state_c.waiting_e[VEHICLE] > 0 && state_c.crossing_n[VEHICLE] == 0 && state_c.crossing_n[PEDESTRIAN] == 0){
		not_waiting_c(VEHICLE, HORIZONTAL);
		sem_post(&turn_h[VEHICLE]);
	}
	else if(state_c.waiting_e[PEDESTRIAN] > 0 && state_c.crossing_n[VEHICLE] == 0){
		not_waiting_c(PEDESTRIAN, HORIZONTAL);
		sem_post(&turn_h[PEDESTRIAN]);
	}
	else{
		sem_post(&light_s);
	}
}
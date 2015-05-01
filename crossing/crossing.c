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
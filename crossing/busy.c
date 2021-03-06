#include "crossing.h"

struct busy state_b;

bool started_b = false;
unsigned int K_b;
sem_t light_b;
sem_t turn_b[3][MAX_TYPE];

void start_b(unsigned int k, size_t MAX_SPAWNS) {
	unsigned int i;
	pthread_t pt;
	bool stop = false;
	if (started_b) {
		fprintf(stderr, "Already started! Aborting.\n");
		return;
	}
	started_b = true;
	K_b = k;
	srand(time(NULL));

	for (i = 0; i < MAX_TYPE; ++i) {
		state_b.crossing[VERTICAL][i] = 0;
		state_b.crossing[HORIZONTAL][i] = 0;
		state_b.crossing[DIAGONAL][i] = 0;
		state_b.waiting[VERTICAL][i] = 0;
		state_b.waiting[HORIZONTAL][i] = 0;
		state_b.waiting[DIAGONAL][i] = 0;
		state_b.k[i] = 1;
		state_b.last[i] = true;
	}
	state_b.k[DIAGONAL] = 1;
	
	sem_init(&light_b, 0, 1);
	sem_init(&turn_b[VERTICAL][PEDESTRIAN], 0, 0);
	sem_init(&turn_b[VERTICAL][VEHICLE], 0, 0);
	sem_init(&turn_b[HORIZONTAL][PEDESTRIAN], 0, 0);
	sem_init(&turn_b[HORIZONTAL][VEHICLE], 0, 0);
	sem_init(&turn_b[DIAGONAL][PEDESTRIAN], 0, 0);

	pthread_create(&pt, NULL, spawner_b, ((void *) MAX_SPAWNS));

	while (PROG_RUNNING && !stop) {
		stop = true;
		milli_sleep(2);	
		for (i = 0; i < MAX_TYPE; ++i) {
			if (atomic_load(&(state_b.waiting[VERTICAL][i]))) stop = false;
			if (atomic_load(&(state_b.crossing[VERTICAL][i]))) stop = false;
			if (atomic_load(&(state_b.waiting[HORIZONTAL][i]))) stop = false;
			if (atomic_load(&(state_b.crossing[HORIZONTAL][i]))) stop = false;
			if (atomic_load(&(state_b.waiting[DIAGONAL][PEDESTRIAN]))) stop = false;
			if (atomic_load(&(state_b.crossing[DIAGONAL][PEDESTRIAN]))) stop = false;
		}
		if (stop) {
			int j;
			_LLB_SEM_WAIT(&light_b);
			for (i = 0; i < 2; ++i) {
				for (j = 0; j < 2; ++j) {
					if (state_b.waiting[i][j] > 0) stop = false;
					if (state_b.crossing[i][j] > 0) stop = false;
				}
			}
			if (state_b.waiting[DIAGONAL][PEDESTRIAN] || state_b.crossing[DIAGONAL][PEDESTRIAN]) stop = false;
			_LLB_SEM_POST(&light_b);
		}
	}

	if (!PROG_RUNNING) {
		pthread_cancel(pt);
		return;
	}

	pthread_join(pt, NULL);
}

void * spawner_b(void * argp) {
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
			pthread_create(&pt, NULL, enter_v_b, NULL);	
			v++;
		} else { 
			pthread_create(&pt, NULL, enter_p_b, NULL);
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

void try_cross_b(unsigned int type, unsigned int dir) {
	bool done = false;;
	_LLB_SEM_WAIT(&light_b);
	if (!can_cross_b(type, dir)) {
		push_button_b(dir);
		waiting_b(type, dir);
		_LLB_SEM_POST(&light_b);
		_LLB_SEM_WAIT(&turn_b[dir][type]);
	}

	if (dec_k_b(type, dir)) { // missing for diagonal
		if (--state_b.k[!dir] == 0) {
			done = true;
		}
	}
	
	inc_cross_b(type, dir);

	if (done) {
		_LLB_SEM_POST(&light_b);
	} else {
		signal_b(type, dir);
	}
	rand_sleep(500);
	_LLB_SEM_WAIT(&light_b);
	done_inc_cross_b(type, dir);
	if (!leaving_crossing_b(type, dir)) {
		log_action("132");
		signal_b(type, dir);
	} else {
		log_action("135");
	}
}

void signal_b(unsigned int type, unsigned int dir) {
	int i,j;
	bool waiting_b[2] = { false, false };
	bool flag1 = false, flag2 = false;
	bool ped_dir = false, ped_nodir = false, veh_dir = false;

	if (dir == DIAGONAL || state_b.crossing[DIAGONAL][PEDESTRIAN] > 0) {
		if (state_b.waiting[HORIZONTAL][PEDESTRIAN] > 0) {
			flag1 = true;
		}
		if (state_b.waiting[VERTICAL][PEDESTRIAN] > 0) {
			flag2 = true;
		}
		state_b.last[1] = !state_b.last[1];
		if (flag1 || flag2) {
			if (state_b.last[1]) {
				if (flag1 && state_b.last[0]) {
					not_waiting_b(PEDESTRIAN, HORIZONTAL);
					_LLB_SEM_POST(&turn_b[HORIZONTAL][PEDESTRIAN]);
					return;
				} else if (flag2) {
					not_waiting_b(PEDESTRIAN, VERTICAL);
					_LLB_SEM_POST(&turn_b[VERTICAL][PEDESTRIAN]);
					return;
				}
			}
		}

		state_b.last[0] = !state_b.last[0];
		if(state_b.waiting[dir][type] && state_b.last[0]){
			not_waiting_b(type, dir);
			_LLB_SEM_POST(&turn_b[dir][type]);
		} else {
			_LLB_SEM_POST(&light_b);
		}
		return;
	}
	
	for (i = 0; i < 2; ++i) {
		for (j=0; j < MAX_TYPE; ++j) {
			if (state_b.waiting[i][j] > 0)
				waiting_b[i] = true;
		}
	}

	if (state_b.k[!dir] == 0 && waiting_b[!dir]) {
		_LLB_SEM_POST(&light_b);
		return;
	} else if (state_b.crossing[!dir][PEDESTRIAN]) {
		// can't send car in dir
		// any pedestrian can go
		flag1 = true;
	} else if (state_b.crossing[dir][VEHICLE]) {
		// can't send ped in !dir
		// only our direction can go
		flag2 = true;
	}

	if (state_b.waiting[dir][PEDESTRIAN]) {
		ped_dir = true;
	}
	if (state_b.waiting[dir][VEHICLE] && !flag1) {
		veh_dir = true;
	}
	if (state_b.waiting[!dir][PEDESTRIAN] && !flag2) {
		ped_nodir = true;
	}

	// competition
	if (ped_dir && (veh_dir || ped_nodir)) {
		state_b.last[0] = !state_b.last[0];
		if (state_b.last[0]) {
			not_waiting_b(PEDESTRIAN, dir);
			_LLB_SEM_POST(&turn_b[dir][PEDESTRIAN]);
			return;
		}
	} else if (ped_dir) {
		not_waiting_b(PEDESTRIAN, dir);
		_LLB_SEM_POST(&turn_b[dir][PEDESTRIAN]);
		return;
	}

	// second competition
	if (veh_dir && ped_nodir) {
		state_b.last[1] = !state_b.last[1];
		if (state_b.last[1]) {
			not_waiting_b(PEDESTRIAN, !dir);
			_LLB_SEM_POST(&turn_b[!dir][PEDESTRIAN]);
			return;
		}
	}

	state_b.last[2] = !state_b.last[2];
	// no actual competition
	if (state_b.waiting[DIAGONAL][PEDESTRIAN] > 0 && state_b.last[2] && can_cross_b(PEDESTRIAN, DIAGONAL)) {
		not_waiting_b(PEDESTRIAN, DIAGONAL);
		_LLB_SEM_POST(&turn_b[DIAGONAL][PEDESTRIAN]);
	} else if (veh_dir) {
		not_waiting_b(VEHICLE, dir);
		_LLB_SEM_POST(&turn_b[dir][VEHICLE]);
	} else if (ped_nodir) {
		not_waiting_b(PEDESTRIAN, !dir);
		_LLB_SEM_POST(&turn_b[!dir][PEDESTRIAN]);
	} else if (state_b.waiting[!dir][VEHICLE] > 0 && !state_b.crossing[dir][VEHICLE] && !state_b.crossing[dir][PEDESTRIAN]) {
		not_waiting_b(VEHICLE, !dir);
		_LLB_SEM_POST(&turn_b[!dir][VEHICLE]);
	} else {
		_LLB_SEM_POST(&light_b);
	}
}

// read as posted after leaving crossing
inline bool leaving_crossing_b(unsigned int type, unsigned int dir) {
	unsigned int i,j;
	bool last = true;

	if (dir == DIAGONAL) {
		if (state_b.crossing[dir][PEDESTRIAN] == 0 && state_b.crossing[VERTICAL][PEDESTRIAN]){
			if(state_b.waiting[HORIZONTAL][VEHICLE]){
				state_b.k[HORIZONTAL] = K_b;
				not_waiting_b(VEHICLE, HORIZONTAL);
				_LLB_SEM_POST(&turn_b[HORIZONTAL][VEHICLE]);
				return true;
			}
			return false;
		}
		else if (state_b.crossing[dir][PEDESTRIAN] == 0 && state_b.crossing[HORIZONTAL][PEDESTRIAN]){
			if(state_b.waiting[VERTICAL][VEHICLE]){
				state_b.k[VERTICAL] = K_b;
				not_waiting_b(VEHICLE, VERTICAL);
				_LLB_SEM_POST(&turn_b[VERTICAL][VEHICLE]);
				return true;
			}
			return false;
		}
	}
	for (i = 0; i < MAX_TYPE; ++i) {
		if (state_b.crossing[dir][i] > 0) last = false;
	}

	if (dir == VERTICAL && last) {
		for (i = 0; i < MAX_TYPE; ++i) {
			for (j = 0; j < MAX_DIR; ++j) {
				if (state_b.waiting[j][i] > 0) {
					not_waiting_b(i,j);
					_LLB_SEM_POST(&turn_b[j][i]);
					return true;
				}
			}
		}
		return false;
	} 

	if (last) {
		if (nobody_waiting()) { return false; }
		if (type == PEDESTRIAN && state_b.crossing[!dir][type] > 0) {
			return false;
		}
		for (i = 0; i < MAX_TYPE; ++i) {
			for (j = 0; j < DIAGONAL; ++i) {
				char msg [MSG_SIZE];
				if (dir == j) continue;
				if (state_b.waiting[j][i] > 0) {
					state_b.k[j] = K_b;
					not_waiting_b(i, j);
					sprintf(msg, "b[j][i]: [%u][%u]", j, i);
					log_action(msg);
					_LLB_SEM_POST(&turn_b[j][i]);
					return true;
				}
			}
		}
	} else if (type == VEHICLE && state_b.crossing[dir][type] == 0 && state_b.waiting[!dir][!type]) {
		state_b.k[!dir] = K_b;
		not_waiting_b(!type, !dir);
		_LLB_SEM_POST(&turn_b[!dir][!type]);
		return true;
	}

	return false;
}

inline void push_button_b(unsigned int dir) {
	unsigned int i;
	if (dir == DIAGONAL){
		if (state_b.waiting[dir][PEDESTRIAN] == 0)
			state_b.k[dir] = K_b;
		return;
	}
	for (i = 0; i < MAX_TYPE; ++i) {
		if (state_b.waiting[dir][i] > 0) return;
	}
	// I'm the first one to wait, push the button
	state_b.k[dir] = K_b; // <- pushing the button
}

inline bool dec_k_b(unsigned int type, unsigned int dir) {
	unsigned int i;	
	if (state_b.k[!dir] == 0) {
		return false;
	}
	// I'm a car, and someone is waiting for me
	for (i = 0; i < MAX_TYPE; ++i) {
		if (type == VEHICLE && state_b.waiting[!dir][i]) {
			return true;
		}
	}
	// I'm a pedestrian, and there's a car waiting for me
	if (type == PEDESTRIAN && state_b.waiting[!dir][VEHICLE]) {
		return true;
	}
	return false;
}

inline bool blocked_b(unsigned int dir) {
	unsigned int i,j;
	for (i = 0; i < MAX_DIR; ++i) {
		if (dir == i) continue;
		for (j = 0; MAX_TYPE; ++j) {
			if (state_b.crossing[i][j] > 0) return true;
		}
	} 
	return true;
} 

inline bool can_cross_b(unsigned int type, unsigned int dir) {
	if(dir == DIAGONAL){
		if(state_b.k[HORIZONTAL] == 0 || state_b.k[VERTICAL] == 0){
			return false;
		}
		else if (state_b.crossing[VERTICAL][VEHICLE] > 0 || state_b.crossing[HORIZONTAL][VEHICLE]){
			return false;
		}
		return true;
	}
	if (state_b.k[!dir] == 0) {
		return false; // TODO: REMOVE
		if (type == VEHICLE) {
			return false; // IF K==0, CAN'T CROSS	
		} else if (state_b.waiting[!dir][!type]) {
			return false;
		}
	}
	if (state_b.crossing[!dir][!type] > 0) {
		return false;
	}
	if (type == VEHICLE && (state_b.crossing[!dir][type] > 0 || state_b.crossing[DIAGONAL][!type])) {
		return false;
	}
	// nothing is stopping me
	return true;
}

inline void inc_cross_b(unsigned int type, unsigned int dir) {
	char *msg = malloc(MSG_SIZE);
	unsigned int crossers = 0;
	state_b.crossing[dir][type] += 1;
	crossers = state_b.crossing[dir][type];
	sprintf(msg, "%u, %u +CROSS [%u crossing]", type, dir, crossers);
	log_actionl(msg, 0);
	free(msg);
}

inline void done_inc_cross_b(unsigned int type, unsigned int dir) { 
	char *msg = malloc(MSG_SIZE);
	state_b.crossing[dir][type] -= 1;
	sprintf(msg, "%u, %u -CROSS [%u crossing]", type, dir, state_b.crossing[dir][type]);
	log_actionl(msg, 0);
	free(msg);
}

inline void waiting_b(unsigned int type, unsigned int dir) {
	char *msg = malloc(MSG_SIZE);
	state_b.waiting[dir][type] += 1;
	sprintf(msg, "%u, %u +wait [%u waiting]", type, dir, state_b.waiting[dir][type]);
	log_actionl(msg, 2);
	free(msg);
}

inline void not_waiting_b(unsigned int type, unsigned int dir) {
	char *msg = malloc(MSG_SIZE);
	state_b.waiting[dir][type] -= 1;
	sprintf(msg, "%u, %u -wait [%u waiting]", type, dir, state_b.waiting[dir][type]);
	log_actionl(msg, 2);
	free(msg);
}

void log_sem_b(const char * append) {
	char * msg = malloc(MSG_SIZE);
	int l, v0, v1, h0, h1, d, dv;
	sem_getvalue(&light_b, &l);
	sem_getvalue(&turn_b[VERTICAL][0], &v0);
	sem_getvalue(&turn_b[VERTICAL][1], &v1);
	sem_getvalue(&turn_b[HORIZONTAL][0], &h0);
	sem_getvalue(&turn_b[HORIZONTAL][1], &h1);
	sem_getvalue(&turn_b[DIAGONAL][0], &d);
	sem_getvalue(&turn_b[DIAGONAL][1], &dv);
	sprintf(msg, "[%2u, %2u]vc, [%2u, %2u]hc, [%2u, %2u]vw, [%2u, %2u]hw, [%2u, %2u]d k: %2u, %2u, S[%d %d %d %d %d %d]%d",
			state_b.crossing[VERTICAL][0],
			state_b.crossing[VERTICAL][1],
			state_b.crossing[HORIZONTAL][0],
			state_b.crossing[HORIZONTAL][1],
			state_b.waiting[VERTICAL][0],
			state_b.waiting[VERTICAL][1],
			state_b.waiting[HORIZONTAL][0],
			state_b.waiting[HORIZONTAL][1],
			state_b.crossing[DIAGONAL][PEDESTRIAN],
			state_b.waiting[DIAGONAL][PEDESTRIAN],
			state_b.k[0],
			state_b.k[1],
			l, v0, v1, h0, h1, d, dv);
	if (append) {
		strncat(msg, append, MSG_SIZE - strlen(msg)-1);
	}
	log_actionl(msg, 12);
	free(msg);
}

bool nobody_waiting() {
	unsigned int i,j;
	for (i = 0; i < MAX_TYPE; ++i) {
		for (j = 0; j < MAX_DIR; ++j) {
			if (state_b.waiting[j][i] > 0) {
				return false;
			}
		}
	}
	return true;
} 


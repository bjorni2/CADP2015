#include "crossing.h"

struct crossing state_c;

bool started_c = false;
unsigned int K_c;
sem_t light_c;
sem_t turn_c[2][MAX_TYPE];

void start_c(unsigned int k, size_t MAX_SPAWNS) {
	unsigned int i;
	pthread_t pt;
	bool stop = false;
	if (started_c) {
		fprintf(stderr, "Already started! Aborting.\n");
		return;
	}
	started_c = true;
	K_c = k;
	srand(time(NULL));

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

	while (PROG_RUNNING && !stop) {
		stop = true;
		milli_sleep(2);	
		for (i = 0; i < MAX_TYPE; ++i) {
			if (atomic_load(&(state_c.waiting[VERTICAL][i]))) stop = false;
			if (atomic_load(&(state_c.crossing[VERTICAL][i]))) stop = false;
			if (atomic_load(&(state_c.waiting[HORIZONTAL][i]))) stop = false;
			if (atomic_load(&(state_c.crossing[HORIZONTAL][i]))) stop = false;
		}
		if (stop) {
			int j;
			_LL_SEM_WAIT(&light_c);
			for (i = 0; i < 2; ++i) {
				for (j = 0; j < 2; ++j) {
					if (state_c.waiting[i][j] > 0) stop = false;
					if (state_c.crossing[i][j] > 0) stop = false;
				}
			}
			_LL_SEM_POST(&light_c);
		}
	}

	if (!PROG_RUNNING) {
		pthread_cancel(pt);
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

void try_cross_c(unsigned int type, unsigned int dir) {
	bool done = false;;
	_LL_SEM_WAIT(&light_c);
	if (!can_cross_c(type, dir)) {
		push_button_c(dir);
		waiting_c(type, dir);
		_LL_SEM_POST(&light_c);
		_LL_SEM_WAIT(&turn_c[dir][type]);
	}

	if (dec_k_c(type, dir)) {
		if (--state_c.k[!dir] == 0) {
			done = true;
		}
	}
	
	inc_cross_c(type, dir);

	if (done) {
		_LL_SEM_POST(&light_c);
	} else {
		signal_c(type, dir);
	}
	rand_sleep(500);
	_LL_SEM_WAIT(&light_c);
	done_inc_cross_c(type, dir);
	if (!leaving_crossing_c(type, dir)) {
		signal_c(type, dir);
	}
}

void signal_c(unsigned int type, unsigned int dir) {
	int i,j;
	bool waiting_c[2] = { false, false };
	bool flag1 = false, flag2 = false;
	bool ped_dir = false, ped_nodir = false, veh_dir = false;

	for (i = 0; i < 2; ++i) {
		for (j=0; j < MAX_TYPE; ++j) {
			if (state_c.waiting[i][j] > 0)
				waiting_c[i] = true;
		}
	}

	if (state_c.k[!dir] == 0 && waiting_c[!dir]) {
		_LL_SEM_POST(&light_c);
		return;
	} else if (state_c.crossing[!dir][PEDESTRIAN]) {
		// can't send car in dir
		// any pedestrian can go
		flag1 = true;
	} else if (state_c.crossing[dir][VEHICLE]) {
		// can't send ped in !dir
		// only our direction can go
		flag2 = true;
	}

	if (state_c.waiting[dir][PEDESTRIAN]) {
		ped_dir = true;
	}
	if (state_c.waiting[dir][VEHICLE] && !flag1) {
		veh_dir = true;
	}
	if (state_c.waiting[!dir][PEDESTRIAN] && !flag2) {
		ped_nodir = true;
	}

	// competition
	if (ped_dir && (veh_dir || ped_nodir)) {
		state_c.last[0] = !state_c.last[0];
		if (state_c.last[0]) {
			not_waiting_c(PEDESTRIAN, dir);
			_LL_SEM_POST(&turn_c[dir][PEDESTRIAN]);
			return;
		}
	} else if (ped_dir) {
		not_waiting_c(PEDESTRIAN, dir);
		_LL_SEM_POST(&turn_c[dir][PEDESTRIAN]);
		return;
	}

	// second competition
	if (veh_dir && ped_nodir) {
		state_c.last[1] = !state_c.last[1];
		if (state_c.last[1]) {
			not_waiting_c(PEDESTRIAN, !dir);
			_LL_SEM_POST(&turn_c[!dir][PEDESTRIAN]);
			return;
		}
	}

	// no actual competition
	if (veh_dir) {
		not_waiting_c(VEHICLE, dir);
		_LL_SEM_POST(&turn_c[dir][VEHICLE]);
	} else if (ped_nodir) {
		not_waiting_c(PEDESTRIAN, !dir);
		_LL_SEM_POST(&turn_c[!dir][PEDESTRIAN]);
	} else {
		_LL_SEM_POST(&light_c);
	}
}

// read as posted after leaving crossing
inline bool leaving_crossing_c(unsigned int type, unsigned int dir) {
	unsigned int i;
	bool last = true;
	for (i = 0; i < MAX_TYPE; ++i) {
		if (state_c.crossing[dir][i] > 0) last = false;
	}

	if (last) {
		for (i = 0; i < MAX_TYPE; ++i) {
			if (state_c.waiting[!dir][i]) {
				state_c.k[!dir] = K_c;
				not_waiting_c(i, !dir);
				_LL_SEM_POST(&turn_c[!dir][i]);
				return true;
			}
		}
	} else if (type == VEHICLE && state_c.crossing[dir][type] == 0 && state_c.waiting[!dir][!type]) {
		state_c.k[!dir] = K_c;
		not_waiting_c(!type, !dir);
		_LL_SEM_POST(&turn_c[!dir][!type]);
		return true;
	}

	return false;
}

inline void push_button_c(unsigned int dir) {
	unsigned int i;
	for (i = 0; i < MAX_TYPE; ++i) {
		if (state_c.waiting[dir][i] > 0) return;
	}
	// I'm the first one to wait, push the button
	state_c.k[dir] = K_c; // <- pushing the button
}

inline bool dec_k_c(unsigned int type, unsigned int dir) {
	unsigned int i;
	if (state_c.k[!dir] == 0) {
		return false;
	}
	// I'm a car, and someone is waiting for me
	for (i = 0; i < MAX_TYPE; ++i) {
		if (type == VEHICLE && state_c.waiting[!dir][i]) {
			return true;
		}
	}
	// I'm a pedestrian, and there's a car waiting for me
	if (type == PEDESTRIAN && state_c.waiting[!dir][VEHICLE]) {
		return true;
	}
	return false;
}

inline bool can_cross_c(unsigned int type, unsigned int dir) {
	if (state_c.k[!dir] == 0) {
		return false; // TODO: REMOVE
		if (type == VEHICLE) {
			return false; // IF K==0, CAN'T CROSS	
		} else if (state_c.waiting[!dir][!type]) {
			return false;
		}
	}
	if (state_c.crossing[!dir][!type] > 0) {
		return false;
	}
	if (type == VEHICLE && state_c.crossing[!dir][type] > 0) {
		return false;
	}
	// nothing is stopping me
	return true;
}

inline void inc_cross_c(unsigned int type, unsigned int dir) {
	char *msg = malloc(MSG_SIZE);
	unsigned int crossers = 0;
	state_c.crossing[dir][type] += 1;
	crossers = state_c.crossing[dir][type];
	sprintf(msg, "%u, %u +CROSS [%u crossing]", type, dir, crossers);
	log_actionl(msg, 0);
	free(msg);
}

inline void done_inc_cross_c(unsigned int type, unsigned int dir) { 
	char *msg = malloc(MSG_SIZE);
	state_c.crossing[dir][type] -= 1;
	sprintf(msg, "%u, %u -CROSS [%u crossing]", type, dir, state_c.crossing[dir][type]);
	log_actionl(msg, 0);
	free(msg);
}

inline void waiting_c(unsigned int type, unsigned int dir) {
	char *msg = malloc(MSG_SIZE);
	state_c.waiting[dir][type] += 1;
	sprintf(msg, "%u, %u +wait [%u waiting]", type, dir, state_c.waiting[dir][type]);
	log_actionl(msg, 2);
	free(msg);
}

inline void not_waiting_c(unsigned int type, unsigned int dir) {
	char *msg = malloc(MSG_SIZE);
	state_c.waiting[dir][type] -= 1;
	sprintf(msg, "%u, %u -wait [%u waiting]", type, dir, state_c.waiting[dir][type]);
	log_actionl(msg, 2);
	free(msg);
}

void log_sem_c(const char * append) {
	char * msg = malloc(MSG_SIZE);
	int l, v0, v1, h0, h1;
	sem_getvalue(&light_c, &l);
	sem_getvalue(&turn_c[VERTICAL][0], &v0);
	sem_getvalue(&turn_c[VERTICAL][1], &v1);
	sem_getvalue(&turn_c[HORIZONTAL][0], &h0);
	sem_getvalue(&turn_c[HORIZONTAL][1], &h1);
	sprintf(msg, "[%3u, %3u]vc, [%3u, %3u]hc, [%3u, %3u]vw, [%3u, %3u]hw, k: %2u, %2u, S[%d %d %d %d %d]",
			state_c.crossing[VERTICAL][0],
			state_c.crossing[VERTICAL][1],
			state_c.crossing[HORIZONTAL][0],
			state_c.crossing[HORIZONTAL][1],
			state_c.waiting[VERTICAL][0],
			state_c.waiting[VERTICAL][1],
			state_c.waiting[HORIZONTAL][0],
			state_c.waiting[HORIZONTAL][1],
			state_c.k[0],
			state_c.k[1],
			l, v0, v1, h0, h1);
	if (append) {
		strncat(msg, append, MSG_SIZE - strlen(msg)-1);
	}
	log_actionl(msg, 12);
	free(msg);
}


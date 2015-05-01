#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simple.h"
#include "busy.h"
#include "crossing.h"
#include "logger.h"
#include "screen.h"

void fatal_error(const char * msg);

int main(int argc, char * argv[]) {
	if (argc < 2) {
		fatal_error("missing argument");
	}
	pthread_t pt_ui;
	FILE *fp = fopen("log.txt", "w");
	init_log(stdout);
	
	// TODO: switch chosen_crossing
	// spawn correct controller

//	pthread_create(&pt_ui, NULL, draw_s, (void *) &state_s);
	start_s(3, 100);
//	pthread_join(pt_ui, NULL);

	return 0;
}


void fatal_error(const char * msg) {
	fprintf(stderr, "Fatar error: %s\n", msg);
	exit(-1);
}

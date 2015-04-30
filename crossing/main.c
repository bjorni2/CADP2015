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
#include "pedestrian.h"
#include "vehicle.h"

void fatal_error(const char * msg);

int main(int argc, char * argv[]) {
	if (argc < 2) {
		fatal_error("missing argument");
	}

	// TODO: switch chosen_crossing
	// spawn correct controller

	start_s(9, 9);

	return 0;
}


void fatal_error(const char * msg) {
	fprintf(stderr, "Fatar error: %s\n", msg);
	exit(-1);
}

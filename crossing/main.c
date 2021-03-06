#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simple.h"
#include "busy.h"
#include "crossing.h"
#include "logger.h"
#include "screen.h"

void fatal_error(const char * msg);
bool is_number(char * str);
void int_handler(int sig);

volatile sig_atomic_t PROG_RUNNING = 1;

int main(int argc, char * argv[]) {
	int i, n1=0, n2=0;
	int k=3, agents=100;
	char mode[16];
	pthread_t pt_ui;
	FILE *fp;
	if (argc < 2) {
		fatal_error("Missing argument.");
	}
	signal(SIGINT, int_handler);
	fp = fopen("log.txt", "w");
	init_log(fp);

	for (i = 0; i < argc; ++i) {
		char arg[16];	
		strncpy(arg, argv[i], 16);
		if (is_number(arg)) {
			if (n1 == 0) {
				n1 = atoi(arg);
			} else {
				n2 = atoi(arg);
			}
		} else {
			strncpy(mode, arg, 16);
		}
	}
	if (n1 > 0) {
		if (n2 > 0) {
			if (n1 > n2) {
				k = n2;
				agents = n1;
			} else {
				k = n1;
				agents = n2;
			}
		} else {
			k = n1;
		}
	}

	if (strncmp(mode, "simple", strlen("simple")) == 0) {
		pthread_create(&pt_ui, NULL, draw_s, (void *) &state_s);
		start_s(k, agents);
	} else if (strncmp(mode, "extended", strlen("ext")) == 0) {
		pthread_create(&pt_ui, NULL, draw_c, (void *) &state_c);
		start_c(k, agents);
	} else if (strncmp(mode, "busy", strlen("busy")) == 0) {
		pthread_create(&pt_ui, NULL, draw_b, (void *) &state_b);
		start_b(k, agents);
	} else {
		fatal_error("Invalid argument. Valid: \"simple\", \"extended\", \"busy\".");
	}
	
	draw_stop();
	pthread_join(pt_ui, NULL);

	fclose(fp);
	return 0;
}

void int_handler(int sig) {
	PROG_RUNNING = 0;
	fprintf(stderr, "Received signal %d, terminating...", sig);
	draw_stop();
	milli_sleep(5000);
	fatal_error("Something went wrong in terminating the program!");
}

void fatal_error(const char * msg) {
	fprintf(stderr, "Fatar error: %s\n", msg);
	exit(-1);
}

bool is_number(char * str) {
	char * p;

	for (p = str; *p != 0; ++p) {
		if (!isdigit(*p)) return false;
	}
	return true;
} 


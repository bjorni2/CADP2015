#include "logger.h"

struct options logging_opts = { .debug = false };

void init_log(FILE * f) {
	logging_opts.file = f;
}

void log_actionl(const char * msg, size_t log_level) {
	size_t i;
	for (i = 0; i < log_level; ++i) {
		fprintf(logging_opts.file, "\t");
	}
	log_action(msg);
}

void log_action(const char * msg) {
	fprintf(logging_opts.file, "%s\n", msg);
}

void log_error(const char * msg) {
	fprintf(stderr, "%s\n", msg);
}

void log_sem_wait(const char * name, int line) {
	char msg [64];
	sprintf(msg, "(LOCKED   %s @ %d)", name, line);
	log_actionl(msg, 3);
}

void log_sem_post(const char * name, int line) {
	char msg [64];
	sprintf(msg, "(RELEASED %s @ %d)", name, line);
	log_actionl(msg, 3);
}

void log_sem_error(const char * name, int line) {
	char msg [64];
	sprintf(msg, "(ERROR    %s @ %d)", name, line);
	log_actionl(msg, 3);
}


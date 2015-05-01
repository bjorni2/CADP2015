#include "logger.h"

struct options logging_opts = { .debug = false };

void log_actionl(const char * msg, size_t log_level) {
	size_t i;
	for (i = 0; i < log_level; ++i) {
		fprintf(stdout, "\t");
	}
	log_action(msg);
}

void log_action(const char * msg) {
	fprintf(stdout, "%s\n", msg);
}

void log_error(const char * msg) {
	fprintf(stderr, "%s\n", msg);
}

#include "logger.h"

struct options logging_opts = { .debug = false };

void log_action(const char * msg) {
	fprintf(stdout, "%s\n", msg);
}

void log_error(const char * msg) {
	fprintf(stderr, "%s\n", msg);
}

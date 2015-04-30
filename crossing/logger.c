#include "logger.h"

// logging_opts.debug = false;

void log_action(const char * msg) {
	fprintf(stdout, "%s\n", msg);
}

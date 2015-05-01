#pragma once

#include <stdbool.h>
#include <stdio.h>

extern struct options {
	bool debug;
	FILE * file;
} logging_opts;

void log_actionl(const char * msg, size_t log_level);
void log_action(const char * msg);
void log_error(const char * msg);


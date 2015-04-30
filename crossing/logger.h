#pragma once

#include <stdbool.h>
#include <stdio.h>

struct options {
	bool debug;
	FILE * file;
} logging_opts;

void log_action(const char * msg);


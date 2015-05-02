#pragma once

#include <stdbool.h>
#include <stdio.h>

extern struct options {
	bool debug;
	FILE * file;
} logging_opts;

void init_log(FILE * f);

void log_actionl(const char * msg, size_t log_level);
void log_action(const char * msg);
void log_error(const char * msg);

void log_sem_wait(const char * name, int line);
void log_sem_post(const char * name, int line);
void log_sem_error(const char * name, int line);


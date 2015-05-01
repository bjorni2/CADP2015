#pragma once 

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "logger.h"
#include "tools.h"
#include "pedestrian.h"
#include "vehicle.h"

struct crossing {
	int a;
} state_c;

void start_c(unsigned int k, size_t MAX_SPAWNS);


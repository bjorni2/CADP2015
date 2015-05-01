#pragma once

#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "tools.h"
#include "simple.h"
#include "crossing.h"
#include "busy.h"

#define WIDTH 79
#define SEG_TOP    0
#define SEG_BOTTOM 1
#define SEG_ROAD   2
#define SEG_ROADS  3
#define SEG_WALK   4

extern volatile bool SCREEN_DRAWING_ENABLED;

void draw_buf(char * buf);
void insert_segment(char * buf, unsigned int seg);
void insert_critical(char *buf, struct simple * s);

void * draw_s(void * argp);
void * draw_c(struct crossing * c);
void * draw_b(struct busy * b);

void stop();


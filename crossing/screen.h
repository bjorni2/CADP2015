#pragma once

#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tools.h"
#include "simple.h"
#include "crossing.h"
#include "busy.h"

#define WIDTH 79
#define LINES 24
#define SEG_TOP    0
#define SEG_BOTTOM 1
#define SEG_ROAD   2
#define SEG_ROADS  3
#define SEG_WALK   4

extern volatile bool SCREEN_DRAWING_ENABLED;

void draw_buf(char * buf);
void make_line(char * line); // empty line (border only)
void make_roadline(char * line);
void make_roadline_h(char * line);
void make_roadline_h2(char * line); // only '|'
void make_roadline_d(char * line);

void ins_to_buf(char * buf, char * line);
void insert_segment(char * buf, unsigned int seg);
void insert_segments(char * buf, unsigned int seg);
void opt_into_line(char * line, unsigned int num);
void opt_side_line(char * line, unsigned int num);
void opt_generic_line(char * line, unsigned int num, unsigned int pos);

void insert_critical_s(char *buf, struct simple * s);
void insert_critical_c1(char * buf, struct crossing * c);
void insert_critical_c2(char * buf, struct crossing * c);
void insert_critical_b1(char *buf, struct busy * b);
void insert_critical_b2(char *buf, struct busy * b);

void * draw_s(void * argp);
void * draw_c(void * argp);
void * draw_b(void * argp);

void draw_simple(char * screen, struct simple * s);
void draw_crossing(char * screen, struct crossing * s);
void draw_busy(char * screen, struct busy * s);

void draw_stop();


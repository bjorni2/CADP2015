#include "screen.h"

volatile bool SCREEN_DRAWING_ENABLED;

void draw_buf(char * buf) {
	fprintf(stdout, "\33[H");
	fprintf(stdout, "%s", buf);
	fflush(stdout);
	memset(buf, 0, WIDTH + 2);
	milli_sleep(16);
}

void insert_segment(char * buf, unsigned int seg) {
	char * line = malloc(WIDTH+1);
	size_t dl = WIDTH - 1;
	memset(line, 0, WIDTH+1);
	switch (seg) {
		case SEG_TOP:
			memset(line, '_', WIDTH);
			memset(line, ' ', 1);
			memset(line + dl, ' ', 1);
			break;
		case SEG_BOTTOM:
			memset(line, '_', WIDTH);
			memset(line, '|', 1);
			memset(line + dl - 1, '|', 1);
			break;
		case SEG_ROAD:
			memset(line, ' ', WIDTH);
			memset(line + 31, '|', 1);
			memset(line + 36, '|', 1);
			memset(line + 41, '|', 1);
			memset(line, '|', 1);
			memset(line + dl - 1, '|', 1);
			break;
		case SEG_ROADS:
			memset(line, ' ', WIDTH);
			memset(line + 31, '|', 1);
			memset(line + 41, '|', 1);
			memset(line, '|', 1);
			memset(line + dl - 1, '|', 1);
			break;
		case SEG_WALK:
			memset(line, ' ', WIDTH);
			memset(line + 30, '_', 13);
			memset(line + 31, '|', 1);
			memset(line + 41, '|', 1);
			memset(line, '|', 1);
			memset(line + dl - 1, '|', 1);
			break;
		default:
			break;
	}
	memset(line + WIDTH-1, '\n', 1);
	memset(line + WIDTH, 0, 1);
	strncat(buf, line, WIDTH);
	free(line);
}

void insert_segments(char * buf, unsigned int seg) {
	int i;
	if (seg == SEG_ROAD) {
		for (i = 0; i < 4; ++i) {
			insert_segment(buf, SEG_ROADS);
			insert_segment(buf, SEG_ROAD);
		}
	}
}

void make_line(char * line) {
	memset(line, 0, WIDTH+1);
	memset(line, ' ', WIDTH);
	memset(line, '|', 1);
	memset(line + WIDTH - 2, '|', 1);
}

void make_roadline(char * line) {
	memset(line, 0, WIDTH+1);
	memset(line, ' ', WIDTH);
	memset(line + 31, '|', 1);
	memset(line + 41, '|', 1);
	memset(line, '|', 1);
	memset(line + WIDTH - 2, '|', 1);
}

void make_roadline_h(char * line) {
	memset(line, 0, WIDTH+1);
	memset(line, '_', WIDTH);
	memset(line + 32, ' ', 9);
	memset(line + 31, '|', 1);
	memset(line + 41, '|', 1);
	memset(line, '|', 1);
	memset(line + WIDTH - 2, '|', 1);
}

void make_roadline_h2(char * line) {
	memset(line + 18, '|', 1);
	memset(line + 25, '|', 1);
	memset(line + 47, '|', 1);
	memset(line + 54, '|', 1);
	memset(line, '|', 1);
	memset(line + WIDTH - 2, '|', 1);
}

void make_roadline_d(char * line) {
	int pos;
	memset(line, 0, WIDTH+1);
	memset(line, ' ', WIDTH);

	for (pos = 2; pos < WIDTH-3; ++pos) {
		if ((pos - 2) % 4 != 0) continue;
		if (pos < 29 && pos > 42)  continue;

		memset(line + pos, '_', 2);
	}

	memset(line + 32, ' ', 9);
	memset(line, '|', 1);
	memset(line + WIDTH - 2, '|', 1);
}

void ins_to_buf(char * buf, char * line) {
	memset(line + WIDTH-1, '\n', 1);
	memset(line + WIDTH, 0, 1);
	strncat(buf, line, WIDTH);
}

void opt_into_line(char * line, unsigned int num) {
	opt_generic_line(line, num, 40);
}

void opt_inside_line(char * line, unsigned int num) {
	opt_generic_line(line, num, 39);
}

void opt_side_line(char * line, unsigned int num) {
	opt_generic_line(line, num, 49);
}

void opt_side2line(char * line, unsigned int num) {
	opt_generic_line(line, num, 24);
}

void opt_generic_line(char * line, unsigned int num, unsigned int pos) {
	while (true) {
		unsigned int digit = num % 10;
		memset(line + pos--, digit + '0', 1);
		num /= 10;
		if (num == 0) break;
	}
}

void insert_critical_s(char * buf, struct simple * s) {
	char * line = malloc(WIDTH+1);
	insert_segment(buf, SEG_WALK);
	make_roadline(line);
	memset(line + 32, 'V', 1);
	memset(line + 43, 'W', 1);
	memset(line + 44, ':', 1);
	opt_into_line(line, s->crossing[1]);
	opt_side_line(line, s->waiting[0]);
	ins_to_buf(buf, line);
	make_roadline(line);
	memset(line + 32, 'P', 1);
	memset(line + 36, '|', 1);
	opt_into_line(line, s->crossing[0]);
	ins_to_buf(buf, line);
	insert_segment(buf, SEG_WALK);
	make_roadline(line);
	memset(line + 32, 'W', 1);
	memset(line + 33, ':', 1);
	opt_into_line(line, s->waiting[1]);
	ins_to_buf(buf, line);
	free(line);
}

void insert_critical_c1(char * buf, struct crossing * c) {
	char * line = malloc(WIDTH+1);
	make_roadline_h(line);
	ins_to_buf(buf, line);
	make_line(line);
	make_roadline_h2(line);
	memset(line + 19, 'V', 1);
	opt_side2line(line, c->crossing[HORIZONTAL][VEHICLE]);
	ins_to_buf(buf, line);
	make_roadline_d(line);
	make_roadline_h2(line);
	memset(line + 19, 'P', 1);
	opt_side2line(line, c->crossing[VERTICAL][PEDESTRIAN]);
	ins_to_buf(buf, line);
	make_line(line);
	make_roadline_h2(line);
	memset(line + 12, 'v', 1);
	memset(line + 13, ':', 1);
	opt_generic_line(line, c->waiting[HORIZONTAL][VEHICLE], 17);
	ins_to_buf(buf, line);
	make_roadline_h(line);
	make_roadline_h2(line);
	memset(line + 31, ' ', 1);
	memset(line + 41, ' ', 1);
	ins_to_buf(buf, line);
	make_roadline(line);
	memset(line + 18, 'p', 1);
	memset(line + 19, ':', 1);
	opt_side2line(line, c->waiting[VERTICAL][PEDESTRIAN]);
	ins_to_buf(buf, line);
	free(line);
}

void insert_critical_c2(char * buf, struct crossing * c) {
	char * line = malloc(WIDTH+1);
	insert_segment(buf, SEG_WALK);
	make_roadline(line);
	memset(line + 32, 'V', 1);
	memset(line + 43, 'p', 1);
	memset(line + 44, ':', 1);
	opt_into_line(line, c->crossing[VERTICAL][VEHICLE]);
	opt_side_line(line, c->waiting[HORIZONTAL][PEDESTRIAN]);
	ins_to_buf(buf, line);
	make_roadline(line);
	memset(line + 32, 'P', 1);
	memset(line + 36, '|', 1);
	opt_into_line(line, c->crossing[HORIZONTAL][PEDESTRIAN]);
	ins_to_buf(buf, line);
	insert_segment(buf, SEG_WALK);
	make_roadline(line);
	memset(line + 32, 'v', 1);
	memset(line + 33, ':', 1);
	opt_into_line(line, c->waiting[VERTICAL][VEHICLE]);
	ins_to_buf(buf, line);
	free(line);
}

void insert_critical_b1(char * buf, struct busy * b) {
	char * line = malloc(WIDTH+1);
	make_roadline(line);
	memset(line + 43, 'p', 1);
	memset(line + 44, ':', 1);
	opt_side_line(line, b->waiting[DIAGONAL][PEDESTRIAN]);
	ins_to_buf(buf, line);
	make_roadline_h(line);
	memset(line + 39, '/', 1);
	ins_to_buf(buf, line);
	make_line(line);
	make_roadline_h2(line);
	memset(line + 19, 'V', 1); 
	// V xxxx
	opt_side2line(line, b->crossing[HORIZONTAL][VEHICLE]);
	memset(line + 37, '/', 1);
	memset(line + 42, '/', 1);
	ins_to_buf(buf, line);
	make_roadline_d(line);
	make_roadline_h2(line);
	memset(line + 19, 'P', 1); 
	// P xxxx
	opt_side2line(line, b->crossing[DIAGONAL][PEDESTRIAN]);
	memset(line + 35, '/', 1);
	memset(line + 40, '/', 1);
	ins_to_buf(buf, line);
	make_line(line);
	make_roadline_h2(line);
	memset(line + 33, '/', 1);
	memset(line + 34, 'P', 1);
	memset(line + 12, 'v', 1);
	memset(line + 13, ':', 1);
	opt_generic_line(line, b->waiting[HORIZONTAL][VEHICLE], 17);
	opt_generic_line(line, b->crossing[DIAGONAL][PEDESTRIAN], 39);
	ins_to_buf(buf, line);
	make_roadline_h(line);
	make_roadline_h2(line);
	memset(line + 31, '/', 1);
	memset(line + 36, '/', 1);
	memset(line + 41, ' ', 1);
	ins_to_buf(buf, line);
	make_roadline(line);
	memset(line + 18, 'p', 1);
	memset(line + 19, ':', 1);
	opt_side2line(line, b->waiting[VERTICAL][PEDESTRIAN]);
	memset(line + 34, '/', 1);
	ins_to_buf(buf, line);
	free(line);
}

void insert_critical_b2(char * buf, struct busy * b) {
	char * line = malloc(WIDTH+1);
	insert_segment(buf, SEG_WALK);
	make_roadline(line);
	memset(line + 32, 'V', 1);
	memset(line + 43, 'p', 1);
	memset(line + 44, ':', 1);
	opt_into_line(line, b->crossing[VERTICAL][VEHICLE]);
	opt_side_line(line, b->waiting[HORIZONTAL][PEDESTRIAN]);
	ins_to_buf(buf, line);
	make_roadline(line);
	memset(line + 32, 'P', 1);
	memset(line + 36, '|', 1);
	opt_into_line(line, b->crossing[HORIZONTAL][PEDESTRIAN]);
	ins_to_buf(buf, line);
	insert_segment(buf, SEG_WALK);
	make_roadline(line);
	memset(line + 32, 'v', 1);
	memset(line + 33, ':', 1);
	opt_into_line(line, b->waiting[VERTICAL][VEHICLE]);
	ins_to_buf(buf, line);
	free(line);
}

void insert_statusline_c(char * buf, struct crossing * c) {
	char * line = malloc(WIDTH+1);
	sprintf(line, "k:[%u, %u]\n", c->k[0], c->k[1]);
	ins_to_buf(buf, line);
	free(line);
}

void insert_statusline_b(char * buf, struct busy * c) {
	char * line = malloc(WIDTH+1);
	sprintf(line, "k:[%u, %u]\n", c->k[0], c->k[1]);
	ins_to_buf(buf, line);
	free(line);
}

void * draw_s(void * argp) {
	struct simple * s = argp;
	char * screen = malloc(WIDTH * LINES);
	fprintf(stdout, "\33[H\33[2J");
//	char * vbuf = malloc(WIDTH * LINES * 10);
//	setbuf(stdout, vbuf);
	SCREEN_DRAWING_ENABLED = true;
	while(atomic_load(&SCREEN_DRAWING_ENABLED)) {
		draw_simple(screen, s);
	}
	draw_simple(screen, s);
	free(screen);
	return NULL;
}

void * draw_c(void * argp) {
	struct crossing * c = argp;
	char * screen = malloc(WIDTH * LINES);
	fprintf(stdout, "\33[H\33[2J");
	SCREEN_DRAWING_ENABLED = true;
	while(atomic_load(&SCREEN_DRAWING_ENABLED)) {
		draw_crossing(screen, c);
	}
	milli_sleep(1000);
	draw_crossing(screen, c);
	free(screen);
	return NULL;
}

void * draw_b(void *argp) {
	struct busy * b = argp;
	char * screen = malloc(WIDTH * LINES);
	fprintf(stdout, "\33[H\33[2J");
	SCREEN_DRAWING_ENABLED = true;
	while(atomic_load(&SCREEN_DRAWING_ENABLED)) {
		draw_busy(screen, b);
	}
	draw_busy(screen, b);
	free(screen);
	return NULL;
}

void draw_simple(char * screen, struct simple * s) {
	insert_segment(screen, SEG_TOP);	
	insert_segments(screen, SEG_ROAD);
	insert_critical_s(screen, s);
	insert_segments(screen, SEG_ROAD);
	insert_segment(screen, SEG_BOTTOM);
	draw_buf(screen);
}

void draw_crossing(char * screen, struct crossing * s) {
	insert_segment(screen, SEG_TOP);	
	insert_segment(screen, SEG_ROAD);
	insert_segment(screen, SEG_ROADS);
	insert_segment(screen, SEG_ROAD);
	insert_segment(screen, SEG_WALK);
	insert_segment(screen, SEG_ROADS);
	insert_segment(screen, SEG_ROAD);
	insert_segment(screen, SEG_WALK);

	insert_critical_c1(screen, s);
	insert_critical_c2(screen, s);

	insert_segment(screen, SEG_ROADS);
	insert_segment(screen, SEG_ROAD);
	insert_segment(screen, SEG_BOTTOM);
	insert_statusline_c(screen, s);
	draw_buf(screen);
}

void draw_busy(char * screen, struct busy * s) { 
	insert_segment(screen, SEG_TOP);	
	insert_segment(screen, SEG_ROAD);
	insert_segment(screen, SEG_ROADS);
	insert_segment(screen, SEG_ROAD);
	insert_segment(screen, SEG_WALK);
	insert_segment(screen, SEG_ROAD);
	insert_segment(screen, SEG_ROAD);
	insert_segment(screen, SEG_WALK);

	insert_critical_b1(screen, s);
	insert_critical_b2(screen, s);

	insert_segment(screen, SEG_ROADS);
	insert_segment(screen, SEG_ROAD);
	insert_segment(screen, SEG_BOTTOM);
	insert_statusline_b(screen, s);
	draw_buf(screen);
}

void draw_stop() {
	atomic_store(&SCREEN_DRAWING_ENABLED, false);
}


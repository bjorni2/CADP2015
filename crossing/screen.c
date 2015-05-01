#include "screen.h"

volatile bool SCREEN_DRAWING_ENABLED;

void draw_buf(char * buf) {
	fprintf(stdout, "%s", buf);
}

void insert_segment(char * buf, unsigned int seg) {
	char * line = malloc(WIDTH + 1);
	switch (seg) {
		case SEG_TOP:
			memset(line, '_', WIDTH);
			memset(line, ' ', 1);
			memset(line + WIDTH - 1, ' ', 1);
			break;
		case SEG_BOTTOM:
			memset(line, '_', WIDTH);
			memset(line, '|', 1);
			memset(line + WIDTH - 1, '|', 1);
			break;
		case SEG_ROAD:
			memset(line, ' ', WIDTH);
			memset(line + 31, '|', 1);
			memset(line + 36, '|', 1);
			memset(line + 41, '|', 1);
			memset(line, '|', 1);
			memset(line + WIDTH - 1, '|', 1);
			break;
		case SEG_ROADS:

			break;
		case SEG_WALK:

			break;
		default:
			break;
	}
	strncpy(line, "\n", 1);
	strncpy(buf, line, WIDTH + 1);
	free(line);
}

void insert_critical(char *buf, struct simple * s) {
	strncpy(buf, "foo", strlen("foo"));
}

void * draw_s(void * argp) {
	struct simple * s = argp;
	char * screen = malloc(4096);
	SCREEN_DRAWING_ENABLED = true;
	while(atomic_load(&SCREEN_DRAWING_ENABLED)) {
		milli_sleep(16);
		insert_segment(screen, SEG_TOP);	
		insert_segment(screen, SEG_ROAD);
		insert_critical(screen, s);
		insert_segment(screen, SEG_ROAD);
		insert_segment(screen, SEG_BOTTOM);
		draw_buf(screen);
	}
	free(screen);
	return NULL;
}

void stop() {
	atomic_store(&SCREEN_DRAWING_ENABLED, false);
}

#pragma once

#include "simple.h"
#include "crossing.h"
#include "busy.h"
#include <ncurses.h>
#include <unistd.h>
#include <stdbool.h>

#define DELAY 30000

void draw_s(struct simple s);
void draw_c(struct crossing c);
void draw_b(struct busy b);


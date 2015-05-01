void draw_s(struct simple s){
	int x = 0, y = 0;
	initscr();
	noecho();
	curs_set(false);
	while(1) {
		clear();             // Clear the screen of all
				     // previously-printed characters
		mvprintw(y, x, "o"); // Print our "ball" at the current xy position
		refresh();
		usleep(DELAY);       // Shorter delay between movements
		x++;                 // Advance the ball to the right
	}
	endwin();
	return;
}

#include <curses.h>
#include <cstdlib>
#include <ctime>

int plrx, plry, score = 0;
int height = 21, width = 37;
const int wall = 0, pass = 1, plr = -1, liveshroom = 2, deadshroom = 3, start = 4, finish = 5;

bool deadend(int x, int y, int** maze, int height, int width) {
	int a = 0;

	if (x != 1) {if (maze[y][x - 2] == pass) {++a;}}
	else ++a;

	if (y != 1) {if (maze[y - 2][x] == pass) {++a;}}
	else ++a;


	if (x != width - 2) {if (maze[y][x + 2] == pass) {++a;}}
	else ++a;

	if (y != height - 2) {if (maze[y + 2][x] == pass) {++a;}}
	else ++a;

	if (a == 4) return 1;
	else return 0;
}

void mazemake(int** maze, int height, int width) {
	int x, y, c, a;
	bool b;

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			maze[i][j] = wall;

	x = 3; y = 3; a = 0;
	while (a < 10000) {
		maze[y][x] = pass; a++;
		while (1) {
			c = rand() % 4;
			switch (c) {
			case 0: if (y != 1)
					if (maze[y - 2][x] == wall) { // Вверх
						maze[y - 1][x] = pass;
						maze[y - 2][x] = pass;
						y -= 2;
					}
			case 1: if (y != height - 2)
					if (maze[y + 2][x] == wall) { // Вниз
						maze[y + 1][x] = pass;
						maze[y + 2][x] = pass;
						y += 2;
					}
			case 2: if (x != 1)
					if (maze[y][x - 2] == wall) { // Налево
						maze[y][x - 1] = pass;
						maze[y][x - 2] = pass;
						x -= 2;
					}
			case 3: if (x != width - 2)
					if (maze[y][x + 2] == wall) { // Направо
						maze[y][x + 1] = pass;
						maze[y][x + 2] = pass;
						x += 2;
					}
			}
			if (deadend(x, y, maze, height, width))
				break;
		}

		if (deadend(x, y, maze, height, width))
			do {
				x = 2 * (rand() % ((width - 1) / 2)) + 1;
				y = 2 * (rand() % ((height - 1) / 2)) + 1;
			}
			while (maze[y][x] != pass);
	}
	maze[plry][plrx] = plr;
}

void initmz(int** maze, int height, int width) {
	mazemake(maze, height, width);
	plrx = 1, plry = 1;
	maze[plrx][plry] = plr;
	maze[0][0] = start;
	maze[height - 2][width - 2] = finish;
}

int move(int** maze, int swch, int* x, int* y) {
	maze[*y][*x] = pass;
	switch (swch) {
	case KEY_UP:
		if (maze[*y - 1][*x] != wall) --*y;
		break;
	case KEY_DOWN:
		if (maze[*y + 1][*x] != wall) ++*y;
		break;
	case KEY_LEFT:
		if (maze[*y][*x - 1] != wall) --*x;
		break;
	case KEY_RIGHT:
		if (maze[*y][*x + 1] != wall) ++*x;
		break;
	case KEY_END: return 2;
	case ALT_X: return -1;
	}
	if (maze[*y][*x] == finish) {
		return 0;
	} else {
		maze[*y][*x] = plr;
		return 1;
	}
}

void visual(int** maze, int score, int height, int width) {
	clear();
	move(0, 0);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			switch (maze[i][j]) {
			case wall: color_set(1, NULL); addstr("# "); break;
			case pass: printw("  "); break;
			case plr: color_set(2, NULL); addstr("+ "); break;
			case finish: color_set(2, NULL); addstr("$ "); break;
			case liveshroom: color_set(3, NULL); addstr("* "); break;
			}
		}
		printw("\n");
	}
	color_set(4, NULL);
	mvprintw(1, width * 2 + 3, "=========");
	mvprintw(2, width * 2 + 3, "| $ = %d |", score);
	mvprintw(3, width * 2 + 3, "=========");
}

void shroom(int** maze, int height, int width) {
	int x, y;
	x = 1; y = 1;

	while (maze[y][x] != finish) {
		maze[y][x] = liveshroom;
		if (maze[y][x + 1] == pass) {
			maze[y][x + 1] = liveshroom;
			x += 2;
			continue;
		}

		if (maze[y][x - 1] == pass) {
			maze[y][x - 1] = liveshroom;
			x -= 2;
			continue;
		}

		if (maze[y + 1][x] == pass) {
			maze[y + 1][x] = liveshroom;
			y += 2;
			continue;
		}

		if (maze[y - 1][x] == pass) {
			maze[y - 1][x] = liveshroom;
			y -= 2;
			continue;
		}

		if (maze[y][x + 1] != pass && maze[y][x - 1] != pass && maze[y + 1][x] != pass && maze[y - 1][x] != pass) {
			maze[y][x] = deadshroom;
			if (maze[y][x + 1] == liveshroom) {
				maze[y][x + 1] = deadshroom;
				x += 2;
				continue;
			}

			if (maze[y][x - 1] == liveshroom) {
				maze[y][x - 1] = deadshroom;
				x -= 2;
				continue;
			}

			if (maze[y + 1][x] == liveshroom) {
				maze[y + 1][x] = deadshroom;
				y += 2;
				continue;
			}

			if (maze[y - 1][x] == liveshroom) {
				maze[y - 1][x] = deadshroom;
				y -= 2;
				continue;
			}
		}
	}
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			if (maze[i][j] == deadshroom)
				maze[i][j] = pass;

}

int main() {
	srand((unsigned)time(NULL));

	int** maze = new int*[height];
	for (int i = 0; i < height; i++) maze[i] = new int[width];

	initscr();
	start_color();
	curs_set(0);
	noecho();
	keypad(stdscr, true);
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_CYAN, COLOR_BLACK);
	init_pair(3, COLOR_RED, COLOR_BLACK);
	init_pair(4, COLOR_BLUE, COLOR_BLACK);
	initmz(maze, height, width);

	while (true) {
		visual(maze, score, height, width);
		switch (move(maze, (int)getch(), &plrx, &plry)) {
		case 0: initmz(maze, height, width); ++score; break;
		case -1:
			endwin();
			for (int i = 0; i < height; i++) delete[] maze[i];
			delete[] maze;
			return 0;
		case 2: shroom(maze, height, width); break;
		}
	}
}
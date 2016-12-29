#include <rlutil.h>

#include <cstdlib> // for srand() / rand()
#include <cstdio>

#include <cmath>

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif // min

/// Tiles
#define FLOOR 0
#define WALL 1
#define COIN (1 << 1)
#define STAIRS_DOWN (1 << 2)
#define TORCH (1 << 4)

#define MAPSIZE 15

/// Globals
int x, y;
int coins = 0, moves = 0, torch = 30, level = 1;
int lvl[MAPSIZE][MAPSIZE];

/// Generates the dungeon map
void gen(int seed) {
	srand(seed);
	int i, j;
	for (j = 0; j < MAPSIZE; j++) {
		for (i = 0; i < MAPSIZE; i++) {
			if (i == 0 || i == MAPSIZE-1 || j == 0 || j == MAPSIZE-1 ||
			  rand() % 10 == 0) lvl[i][j] = 1;
			else if (rand() % 20 == 0) lvl[i][j] = COIN;
			else if (rand() % 100 == 0) lvl[i][j] = TORCH;
			else lvl[i][j] = 0;
		}
	}
	#define randcoord (1+rand()%(MAPSIZE-2))
	x = randcoord;
	y = randcoord;
	lvl[randcoord][randcoord] = STAIRS_DOWN;
	#undef randcoord
}

/// Draws the screen
void draw() {
	rlutil::cls();
	rlutil::locate(1, MAPSIZE + 1);
	rlutil::setColor(rlutil::YELLOW);
	printf("Coins: %d\n", coins);
	rlutil::setColor(rlutil::RED);
	printf("Torch: %d\n", torch);
	rlutil::setColor(rlutil::MAGENTA);
	printf("Moves: %d\n", moves);
	rlutil::setColor(rlutil::GREEN);
	printf("Level: %d\n", level);
	rlutil::locate(1, 1);
	int i, j;
	for (j = 0; j < MAPSIZE; j++) {
		for (i = 0; i < MAPSIZE; i++) {
			if (0); //(i == x && j == y) printf("@");
			else if (abs(x-i)+abs(y-j)>min(10,torch/2)) printf(" ");
			else if (lvl[i][j] == 0) { rlutil::setColor(rlutil::BLUE); printf("."); }
			else if (lvl[i][j] & WALL) { rlutil::setColor(rlutil::CYAN); printf("#"); }
			else if (lvl[i][j] & COIN) { rlutil::setColor(rlutil::YELLOW); printf("o"); }
			else if (lvl[i][j] & STAIRS_DOWN) { rlutil::setColor(rlutil::GREEN); printf("<"); }
			else if (lvl[i][j] & TORCH) { rlutil::setColor(rlutil::RED); printf("f"); }
		}
		printf("\n");
	}
	rlutil::locate(x+1, y+1);
	rlutil::setColor(rlutil::WHITE);
	printf("@");
	fflush(stdout);
}

/// Main loop and input handling
int main() {
	rlutil::hidecursor();
	rlutil::saveDefaultColor();
	gen(level);
	rlutil::setColor(2);
	printf("Welcome! Use WASD to move, ESC to quit.\n");
	rlutil::setColor(6);
	rlutil::anykey("Hit any key to start.\n");
	draw();
	while (1) {
		// Input
		if (kbhit()) {
			char k = rlutil::getkey();

			int oldx = x, oldy = y;
			if (k == 'a') { --x; ++moves; }
			else if (k == 'd') { ++x; ++moves; }
			else if (k == 'w') { --y; ++moves; }
			else if (k == 's') { ++y; ++moves; }
			else if (k == rlutil::KEY_ESCAPE) break;
			// Collisions
			if (lvl[x][y] & WALL) { x = oldx; y = oldy; }
			else if (lvl[x][y] & COIN) { coins++; lvl[x][y] ^= COIN; }
			else if (lvl[x][y] & TORCH) { torch+=20; lvl[x][y] ^= TORCH; }
			else if (lvl[x][y] & STAIRS_DOWN) gen(++level);
			// Drawing
			draw();
			// Die
			if (--torch <= 0) break;
		}
	}

	rlutil::cls();
	rlutil::resetColor();
	rlutil::showcursor();

	return 0;
}

// i sawed this boat in half
// and fixed it using only flex tape
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
// colors
#define BLACK 30
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
#define MAGENTA 35
#define CYAN 36
#define WHITE 37
#define DEFAULT_COL WHITE
// tile nums
#define T_AIR 0
#define T_WALL 1
#define T_BORDER 2 // solid wall at the edge of map
#define T_LAVA 3
// item nums
#define I_NOTHING 0
// generic (red potion, yew wand)
#define I_POTION_RED 1
#define I_POTION_GREEN 2
// specific (healing potion, lightning wand)
#define I_POTION_DEATH 3
#define I_POTION_NOTHING 4

#define NUM_OF_IDEN_ITEMS 2
// 447135


#define D_WIDTH 20
#define D_HEIGHT 20

int GENERICS[NUM_OF_IDEN_ITEMS] = {I_POTION_RED,I_POTION_GREEN};
int SPECIFICS[NUM_OF_IDEN_ITEMS] = {I_POTION_DEATH, I_POTION_NOTHING};
char *times(char *str, int reps) { // repeats a string a certain number of times.
	char *buf = (char *)malloc(1024);
	*buf = 0;
	for (int i = 0; i < reps; i++) {
		buf = strcat(buf,str);
	}
	return buf;
}
void printc(char *str, int color) { // color printing, uses DEFINE macros for colors
	printf("\x1b\x5b%dm",color);
	printf("%s",str);
	printf("\x1b\x5b%dm",DEFAULT_COL);
}
void color_set(int color) { // enables a color
	printf("\x1b\x5b%dm",color);
}
void color_reset() { // back to default
	printf("\x1b\x5b%dm",DEFAULT_COL);
}
// some ANSI escape sequences
char *ANSI_up = "\x1b\x5b""A";
char *ANSI_down = "\x1b\x5b""B";
char *ANSI_right = "\x1b\x5b""C";
char *ANSI_left = "\x1b\x5b""D";
char *ANSI_next = "\x1b\x5b""E";
char *ANSI_prev = "\x1b\x5b""F";
char *ANSI_clr = "\x1b\x5b""2J";
char *ANSI_clr_ln = "\x1b\x5b""2K";
char *ANSI_scr_up = "\x1b\x5b""S";
char *ANSI_scr_down = "\x1b\x5b""T";
char *ANSI_home = "\x1b\x5b""H";
// tileset
char TILES[] = " ##:";
int T_COLORS[] = {WHITE,WHITE,BLUE,RED};
#define SOLID 1
int T_FLAGS[] = {0,SOLID,SOLID,0}; 
// itemset and items on map
int ITEMS[D_WIDTH][D_HEIGHT];
char *ITEM_CHARS = " !";
int *ITEM_COLS = {WHITE,GREEN};

// dungeon
int dungeon[D_WIDTH][D_HEIGHT];
// player
int player_x;
int player_y;
void enforce_borders() { // set the edge tiles to T_BORDER
	for (int y = 0; y < D_HEIGHT; y++) {
		if (y == 0 || y == D_HEIGHT-1) { // top or bottom need all tiles T_BORDER
			for (int x = 0; x < D_WIDTH; x++) {
				dungeon[y][x] = T_BORDER;
			}
		} else {
			dungeon[y][0] = T_BORDER;
			dungeon[y][D_HEIGHT-1] = T_BORDER;
		}
	}
}
int fill_tiles(int left_x, int top_y, int width, int height, int fill_with) { // a rectangle will be filled
	if (left_x < 0 || top_y < 0 || width < 0 || height < 0 || left_x + width > D_WIDTH || top_y + height > D_HEIGHT) {
		return 1; // invalid position
	} else { // valid position
		for (int x = left_x; x < left_x + width; x++) {
			for (int y = top_y; y < top_y + height; y++) {
				dungeon[y][x] = fill_with; // the tile to fill with
			}
		}
		return 0;
	}
}
void display_dungeon(int left_x, int top_y) { 
	// doesn't do something other than what you think it doesn't not do
	// at a position on the display other than that not denoted by (left_x, top_y)
	// but remember, it doesn't do something other than that
	
	printf("%s", ANSI_home);
	printf("%s%s", times(ANSI_right, left_x), times(ANSI_down, top_y));
	// now we're at the starting position
	for (int i = 0; i < D_HEIGHT; i++) {
		for (int j = 0; j < D_WIDTH; j++) {
			color_set(T_COLORS[dungeon[i][j]]);
			printf("%c",TILES[dungeon[i][j]]);
			color_reset();
		}
		printf("\n");
	}
}
void display_player() {
	printf("%s", ANSI_home);
	printf("%s%s", times(ANSI_right, player_x), times(ANSI_down, player_y));
	printf("@");
	printf("%s",times(ANSI_next,D_HEIGHT-player_y));
}
// retrieved from site: https://stackoverflow.com/questions/7469139/what-is-equivalent-to-getch-getche-in-linux
// not written by us
#include <termios.h>

static struct termios old, new;
/* Initialize new terminal i/o settings */
void initTermios(int echo) 
{
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  new = old; /* make new settings same as old settings */
  new.c_lflag &= ~ICANON; /* disable buffered i/o */
  if (echo) {
      new.c_lflag |= ECHO; /* set echo mode */
  } else {
      new.c_lflag &= ~ECHO; /* set no echo mode */
  }
  tcsetattr(0, TCSANOW, &new); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) 
{
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) 
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

// back to our stuff
// just some bools to make life easier
typedef int bool;
#define true 1;
#define false 0;
bool tile_flag(int tile_x, int tile_y, int test_flag) {
	return !!(T_FLAGS[dungeon[tile_y][tile_x]] & test_flag);
}
// info messages
char *infos[5] = {"-","-","-","-","-"};
void pr_info(char *msg) {
	char *s1 = (char *)malloc(128); // temp strings
	char *s2 = (char *)malloc(128); // use malloc so they don't go out of scope
	char *s3 = (char *)malloc(128);
	char *s4 = (char *)malloc(128);
	char *s5 = (char *)malloc(128);
	strcpy(s1,infos[1]); // shift one place back
	strcpy(s2,infos[2]);
	strcpy(s3,infos[3]);
	strcpy(s4,infos[4]);
	strcpy(s5,msg); // message is last place
	infos[0] = s1; // put back in infos[]
	infos[1] = s2;
	infos[2] = s3;
	infos[3] = s4;
	infos[4] = s5;
}
int death = 0;
bool important_msg = false;
char *death_reason;
int iden_generics[NUM_OF_IDEN_ITEMS];
int iden_specifics[NUM_OF_IDEN_ITEMS];
bool is_contained_in(int arr[], int size, int val) {
	for (int idx = 0; idx < size; idx++) {
		if (arr[idx] == val) {
			return true;
		}
	}
	return false;
}
void shuffle_rand_items() { // generates identifiable items
	for(int i = 0; i < NUM_OF_IDEN_ITEMS; i++) {

	}
}
int main() {
	srand(time(NULL));
	shuffle_rand_items();
	printf("%s",ANSI_clr);
	printc("Welcome to YACrogue!\nMade by Vijay Shanmugam and Joshua Piety\nCollect the mighty Amulet of John Doe from the 26th floor of the dungeon!\nHave fun! (press a key to start)",GREEN);
	getch_(0);
	if (fill_tiles(0,0,D_WIDTH,D_HEIGHT,T_AIR)) {
		printc("ERROR: initial airfill failed!\n",RED);
		return 1;
	}
	dungeon[6][6] = T_LAVA;
	bool game_running = true;
	player_x = 1;
	player_y = 1;
	pr_info("Welcome to INROGUE!");
	while (game_running) {
		enforce_borders();
		printf("%s",ANSI_clr);
		display_dungeon(0,0);
		display_player();
		// print messages
		for (int i = 0; i < 5; i++) {
			printf("%s\n",infos[i]);
		}
		if (important_msg) {
			getch_(0);
			important_msg = false;
		}
		char keypress = getch_(0);
		if (death) {
			;
		} else if (keypress == '\x1b') { // appears to be an ANSI escape sequence
			keypress = getch_(0);
			if (keypress == '\x5b') { // which it is!
				keypress = getch_(0); // meat of the escape sequence
				switch (keypress) {
					case 'A': // up arrow
					if (!tile_flag(player_x, player_y - 1, SOLID)) player_y--;
					break;
					case 'B': // down arrow
					if (!tile_flag(player_x, player_y + 1, SOLID)) player_y++;
					break;
					case 'C': // right arrow
					if (!tile_flag(player_x + 1, player_y, SOLID)) player_x++;
					break;
					case 'D': // left arrow
					if (!tile_flag(player_x - 1, player_y, SOLID)) player_x--;
					break;
				}
			} else if (keypress == '\x1b') { // ...or the user pressed ESC
				printc("\nWould you like to quit? type @ to confirm: ",RED);
				keypress = getch_(1);
				if (keypress == '@') {
					game_running = false;
					printc("\nOh, well. Bye!\n",RED);
				} else {
					printc("\nReally? NO? Wimp!\n",RED);
				}
				getch_(0);
			}
		}
		// check what you're standing on
		int tile_on = dungeon[player_y][player_x];
		if (!death) {
			if (tile_on == T_LAVA) {
				// burn to death
				death = 1;
				pr_info("You burned in lava.");
				death_reason = "burning in lava";
			}
		}	
		if (death == 3) {
			game_running = false;
		} else if (death == 2) {
			death = 3;
			pr_info("You died.");
			important_msg = true;
		} else if (death == 1) {
			death = 2;
		}
	}
	if (death) { // did you die, or did the user quit?
		printf("%s",ANSI_clr);
		color_set(RED);
		printf("GAME OVER\nYou died on level 8 of the dungeon.\nYou were killed by %s.\nPress any key to exit the game\n",death_reason);
		color_reset();
		getch_(0);
	}
}

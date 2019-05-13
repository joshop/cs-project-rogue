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
#define DEFAULT_COL BLACK

#define VISION_RANGE 5
#define OMNISCIENCE 0
// spell nums, data
#define SP_NOTHING 0
#define SP_MISSILE 1
#define SP_SPEED 2
#define SP_BLINK 3
#define SP_HEAL 4
char *spellNames[] = {"nothing","magic missile","speed","teleport","heal self"};
int spellCosts[] = {0,5,10,10,10};
// tile nums
#define T_AIR 0
#define T_WALL 1
#define T_BORDER 2
#define T_LAVA 3
#define T_UPS 4
#define T_DOWNS 5
#define T_LASTPH 6
// other plants continue from here
char *plantName(int id) {
	// leaf twig stem root fruit
	char *s = (char *)malloc(20);
	if (id == 5) s = "grass";
	else {
		char *a;
		int z = (id-6)/5;
		if (z == 0) a = "alpha";
		if (z == 1) a = "beta";
		if (z == 2) a = "gamma";
		if (z == 3) a = "delta";
		if (z == 4) a = "epsilon";
		if (z == 5) a = "zeta";
		if (z == 6) a = "eta";
		if (z == 7) a = "theta";
		if (z == 8) a = "iota";
		if (z == 9) a = "kappa";
		if (z == 10) a = "lambda";
		if (z == 11) a = "mu";
		if (z == 12) a = "nu";
		if (z == 13) a = "xi";
		if (z == 14) a = "omicron";
		if (z == 15) a = "pi";
		if (z == 16) a = "rho";
		if (z == 17) a = "sigma";
		if (z == 18) a = "tau";
		if (z == 19) a = "upsilon";
		if (z == 20) a = "phi";
		if (z == 21) a = "hi";
		if (z == 22) a = "psi";
		if (z == 23) a = "omega";
		z = (id-6)%5;
		strcat(s,a);
		if (z == 0) a = "leaf";
		if (z == 1) a = "twig";
		if (z == 2) a = "stem";
		if (z == 3) a = "root";
		if (z == 4) a = "fruit";
		strcat(s,a);
	} return s;
}
// item nums
#define I_NOTHING 0
// generic (red potion, yew wand)
#define I_POTION_RED 1
#define I_POTION_GREEN 2
#define I_POTION_BLUE 3
#define I_POTION_YELLOW 4
#define I_POTION_ORANGE 5
#define I_POTION_PURPLE 6
#define I_POTION_CYAN 7
#define I_POTION_MAGENTA 8
#define I_POTION_WHITE 9
#define I_POTION_BLACK 10
// specific (healing potion, lightning wand)
// potions are named by BLESSED effect
#define I_POTION_HEAL 11
#define I_POTION_SPEED 12
#define I_POTION_INVIS 13
#define I_POTION_STRENGTH 14
#define I_POTION_MAXHP 15
#define I_POTION_FIREP 16
#define I_POTION_COLDP 17
#define I_POTION_GHOST 18
#define I_POTION_REGEN 19
#define I_POTION_CURE 20
#define I_SCROLL_ENCH 21

#define MAXITEM 27
// neither (armor and weapons)
#define I_ARMOR_1 22
#define I_WEAPON_1 23
#define I_SPELLBOOK_MM 24
#define I_SPELLBOOK_SP 25
#define I_SPELLBOOK_BL 26
#define I_SPELLBOOK_HP 27
#define I_FOOD 669
// conditions
#define COND_NULL 0
#define COND_SPEED 1
#define COND_SLOW 2
#define COND_NOMOVE 3
#define COND_NOSEE 4
#define COND_WEAK 5
#define COND_BLIND 6
int ARMOR_SS[] = {0,10,11,12};
int WEAPON_SS[] = {0,10,11,12};
#define NUM_OF_IDEN_ITEMS 10
#define NUM_CATS 1
// 447135
#define MONSTER_BASIC 1
#define KOBOLD 2
int MONST_STRS[] = {0,10,9};
int MONST_MAX[] = {0,20,20};
#define D_WIDTH 50
#define D_HEIGHT 50

int FIRST_GENERICS[] = {I_POTION_RED,I_POTION_HEAL};

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
void faint(int set) {
	printf("\x1b\x5b%dm",set ? 2 : 0);
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
char TILES[256] = ".##:<>";
int T_COLORS[256] = {WHITE,WHITE,BLUE,RED,BLACK,BLACK};
#define SOLID 1
#define PLANT 2
int T_FLAGS[256] = {0,SOLID,SOLID,0,0,0};
char *T_NAMES[256] = {"air","wall","solid wall","lava","upstairs","downstairs"};
void initPlants() {
	for (int i = T_LASTPH; i<T_LASTPH+119; i++) {
		TILES[i] = '"';
		T_COLORS[i] = GREEN;
		T_FLAGS[i] = PLANT;
		T_NAMES[i] = plantName(i);
	}
}
// itemset and items on map
int items[D_WIDTH][D_HEIGHT];
int enchs[D_WIDTH][D_HEIGHT];
char ITEM_CHARS[256] = " !!!!!!!!!!!!!!!!!!!!?@/####";
int ITEM_COLS[256] = {WHITE,RED,GREEN,BLUE,YELLOW,RED,MAGENTA,BLUE,MAGENTA,WHITE,BLACK,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,GREEN,WHITE,WHITE,RED,GREEN,BLUE,MAGENTA};
char *ITEM_NAMES[256] = {"nothing","red potion","green potion","blue potion","yellow potion","orange potion","purple potion","cyan potion","magenta potion","white potion","black potion","potion of healing","potion of speed","potion of invisibility","potion of strength","potion of endurance","potion of fire resistance","potion of cold resistance","potion of incorporeality","potion of regeneration","potion of curing","scroll of enchantment","leather armor","dagger","spellbook of magic missile","spellbook of speed","spellbook of teleport","spellbook of heal self"};
// dungeon
int dungeon[D_WIDTH][D_HEIGHT];

// just some bools to make life easier
typedef int bool;
#define true 1;
#define false 0;
bool tile_flag(int tile_x, int tile_y, int test_flag) { // retrieve tile info
	return !!(T_FLAGS[dungeon[tile_y][tile_x]] & test_flag);
}

// monsters
int monsters[D_WIDTH][D_HEIGHT];
int mdamages[D_WIDTH][D_HEIGHT];
char *MONST_CHARS = " Bk";
int MONST_COLS[] = {WHITE,RED,RED};
char *MONST_NAMES[] = {"","random thing","kobold"};

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
// seen locations
int seen[D_WIDTH][D_HEIGHT];
void see(int x, int y) {
	seen[y][x] = dungeon[y][x];
}
int visarray[D_WIDTH][D_HEIGHT];
bool isSeen(int x, int y) {
	return seen[y][x] != -1;
}
void display_dungeon(int left_x, int top_y) { 
	// DISPLAYS THE DUNGEON (wow)
	printf("%s%s%s%s", ANSI_home, ANSI_down, times(ANSI_right, left_x), times(ANSI_down, top_y));
	// now we're at the starting position
	for (int i = 0; i < D_HEIGHT; i++) {
		for (int j = 0; j < D_WIDTH; j++) {
			if (visarray[i][j]) {
				color_set(T_COLORS[dungeon[i][j]]);
				printf("%c",TILES[dungeon[i][j]]);
				color_reset();
			} else if (isSeen(j,i)) {
				color_set(T_COLORS[dungeon[i][j]]);
				faint(1);
				printf("%c",TILES[seen[i][j]]);
				color_reset();
				faint(0);
			} else {
				printf(" ");
			}
		}
		printf("\n");
	}
}
void display_player() {
	printf("%s%s", ANSI_home, ANSI_down); // down to skip HP bar
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
bool near(int ax, int ay, int bx, int by) {
	return (ax == bx && abs(ay-by)==1) || (ay == by && abs(ax-bx)==1);
}
bool is_contained_in(int arr[], int size, int val) { // is the val contained in the arr?
	for (int idx = 0; idx < size; idx++) {
		if (arr[idx] == val) {
			return idx+1;
		}
	}
	return 0;
}
void shuffle_rand_items() { // generates identifiable items
	for(int i = 0; i < NUM_OF_IDEN_ITEMS; i++) {
		int cat_num = 0;
		while (i+FIRST_GENERICS[0] >= FIRST_GENERICS[cat_num]) {
			cat_num++; // find which category generic is in
		}
		cat_num--;
		int min_val = FIRST_GENERICS[cat_num]; // first item of category
		int max_val = cat_num+1 == NUM_CATS ? NUM_OF_IDEN_ITEMS : FIRST_GENERICS[cat_num+1]-1; // last item of category
		int rand_val;
		do {
			rand_val = rand()%(max_val-min_val+1)+min_val;
		} while (is_contained_in(iden_specifics, NUM_OF_IDEN_ITEMS, rand_val+NUM_OF_IDEN_ITEMS)); // while value is already used
		iden_generics[i] = i+FIRST_GENERICS[0]; // whichever item operating on
		iden_specifics[i] = rand_val+NUM_OF_IDEN_ITEMS; // corresponding specific
	}
}
void display_generic_specific() { // solely debug
	for (int i = 0; i < NUM_OF_IDEN_ITEMS; i++) {
		color_set(MAGENTA);
		printf("GENERIC: %d, SPECIFIC: %d\n",iden_generics[i],iden_specifics[i]);
		color_reset();
	}
}
// identification
int known_things[200];
int num_known = 0;
void learn(int thing) {
	known_things[num_known++] = thing;
}
char *str_format(char *str_pl, int article, int quantity); // forward declaration
//inventory
int pack_items[20];
int pack_counts[20];
int pack_enchs[20];
int num_pack_slots = 0;
void del_item(int item, int quantity) { // delete items from the pack
	// THIS FUNCTION DOESN'T WORK, WILL BE FIXED
	// i think it works now (?)
	char buffer[255];
	if (!is_contained_in(pack_items,num_pack_slots,item)) {
		pr_info("You don't have that.");
	} else {
		if (pack_counts[is_contained_in(pack_items,num_pack_slots,item)-1] < quantity) { // not enough to remove that many
			pr_info("You don't have enough of that to remove that many!");
		} else {
			sprintf(buffer,"You now have %s.",str_format(ITEM_NAMES[item],1,pack_counts[is_contained_in(pack_items,num_pack_slots,item)-1]-quantity));
			pr_info(buffer);
			if (pack_counts[is_contained_in(pack_items,num_pack_slots,item)-1] == quantity) {
				int idx_of = is_contained_in(pack_items,num_pack_slots,item);
				for (int i = idx_of; i < num_pack_slots; i++) {
					pack_items[i-1] = pack_items[i];
					pack_counts[i-1] = pack_counts[i];
				}
				num_pack_slots--;
			} else {
				pack_counts[is_contained_in(pack_items,num_pack_slots,item)-1] -= quantity; // take some from the stack
			}
		}
	}
}
void add_item(int item, int quantity, int ench) { // add an item to your pack
	char buffer[255];
	if (is_contained_in(pack_items,num_pack_slots,item)) { // already exists
    int prev_amt = pack_counts[is_contained_in(pack_items,num_pack_slots,item)-1];
		pack_counts[is_contained_in(pack_items,num_pack_slots,item)-1] += quantity; // add some more to the stack
		sprintf(buffer,"You now have %s.",str_format(ITEM_NAMES[item],0,prev_amt+quantity));
		pr_info(buffer);
	} else { // new item
		if (num_pack_slots == 20) {
			pr_info("You don't have enough space in your pack.");
		} else {
			pack_items[num_pack_slots] = item;
			pack_counts[num_pack_slots] = quantity;
			pack_enchs[num_pack_slots] = ench;
			num_pack_slots++;
			sprintf(buffer,"Item #%d: %s.",num_pack_slots,str_format(ITEM_NAMES[item],1,quantity));
			pr_info(buffer);
		}
	}
}
void take_inventory(int empty_lines) {
	printf("%s%sInventory: %d items\n",times(ANSI_down, empty_lines), ANSI_home,num_pack_slots);
	for (int i = 0; i < num_pack_slots; i++) {
		printf("Item %d: %s\n",i+1, str_format(ITEM_NAMES[pack_items[i]],1,pack_counts[i]));
	}
}
void display_items(int itemlist[D_HEIGHT][D_WIDTH]) {
	for (int y = 0; y < D_HEIGHT; y++) {
		for (int x = 0; x < D_WIDTH; x++) {
			if (itemlist[y][x]) { // an item exists there
				// scan for iden items
				if (is_contained_in(known_things, num_known, itemlist[y][x])) {
					itemlist[y][x] = iden_specifics[is_contained_in(iden_generics, NUM_OF_IDEN_ITEMS, itemlist[y][x])-1];
				}
				char ic = ITEM_CHARS[itemlist[y][x]];
				printf("%s%s", ANSI_home, ANSI_down); // skip HP bar
				printf("%s%s", times(ANSI_right, x), times(ANSI_down, y));
				color_set(ITEM_COLS[itemlist[y][x]]);
				if (visarray[y][x]) {
					printf("%c",ic);
				} else {
					printf(" ");
				}
				color_reset();
				printf("%s",times(ANSI_next,D_HEIGHT-player_y));
			}
		}
	}
	// also scan for pack iden items
	for (int i = 0; i < num_pack_slots; i++) {
		if (is_contained_in(known_things, num_known, pack_items[i]) && pack_items[i] <= NUM_OF_IDEN_ITEMS) {
			pack_items[i] = iden_specifics[is_contained_in(iden_generics, NUM_OF_IDEN_ITEMS, pack_items[i])-1];
		}
	}
}
void display_monsts(int monstlist[D_HEIGHT][D_WIDTH]) { // copy of items pretty much
	for (int y = 0; y < D_HEIGHT; y++) {
		for (int x = 0; x < D_WIDTH; x++) {
			if (monstlist[y][x]) { // a monster exists there
				// scan for dead monsters also
				if (mdamages[y][x] >= MONST_MAX[monstlist[y][x]]) {
					char buf[256];
					sprintf(buf,"The %s dies!",MONST_NAMES[monstlist[y][x]]);
					pr_info(buf);
					monstlist[y][x] = 0;
				}
				char mc = MONST_CHARS[monstlist[y][x]];
				printf("%s%s", ANSI_home, ANSI_down); // skip HP bar
				printf("%s%s", times(ANSI_right, x), times(ANSI_down, y));
				color_set(MONST_COLS[monstlist[y][x]]);
				if (visarray[y][x]) {
					printf("%c",mc);
				} else {
					printf(" ");
				}
				color_reset();
				printf("%s",times(ANSI_next,D_HEIGHT-player_y));
			}
		}
	}
}
int armor_on = 0;
int weapon_on = 0;
int weapon_en = 0;
int armor_en = 0;
int playerStr = 10;
int maxhp = 100;

int maxpow = 10;
int power = 10;
int knownSpells[10];
int numSpells = 0;
void spMagicMissile() {
	printf("%sYou release a magic missile!\nWhich direction? (arrows)",ANSI_home);
	char x = getch_(0);
	int locx = player_x;
	int locy = player_y;
	do {
		x = getch_(0);
	} while (x != '\x1b');
	x = getch_(0);
	if (x == '\x5b') {
		power -= 5;
		switch(getch_(0)) {
		case 'A':
			locy--;
			while (!monsters[locy][locx] && !tile_flag(locx,locy,SOLID)) {
				printf("%s%s%s|",ANSI_home,times(ANSI_right,locx),times(ANSI_down,locy+1));
				locy--;
			}
			do {
				x = getch_(0);
			} while (x != '\n');
			if (tile_flag(locx,locy,SOLID)) {
				pr_info("The magic missile hits a wall!");
			} else {
				pr_info("The magic missile hits a foe!");
				mdamages[locy][locx] += 30;
			}
			break;
		case 'B':
			locy++;
			while (!monsters[locy][locx] && !tile_flag(locx,locy,SOLID)) {
				printf("%s%s%s|",ANSI_home,times(ANSI_right,locx),times(ANSI_down,locy+1));
				locy++;
			}
			do {
				x = getch_(0);
			} while (x != '\n');
			if (tile_flag(locx,locy,SOLID)) {
				pr_info("The magic missile hits a wall!");
			} else {
				pr_info("The magic missile hits a foe!");
				mdamages[locy][locx] += 30;
			}
			break;
		case 'C':
			locx++;
			while (!monsters[locy][locx] && !tile_flag(locx,locy,SOLID)) {
				printf("%s%s%s-",ANSI_home,times(ANSI_right,locx),times(ANSI_down,locy+1));
				locx++;
			}
			do {
				x = getch_(0);
			} while (x != '\n');
			if (tile_flag(locx,locy,SOLID)) {
				pr_info("The magic missile hits a wall!");
			} else {
				pr_info("The magic missile hits a foe!");
				mdamages[locy][locx] += 30;
			}
			break;
		case 'D':
			locx--;
			while (!monsters[locy][locx] && !tile_flag(locx,locy,SOLID)) {
				printf("%s%s%s-",ANSI_home,times(ANSI_right,locx),times(ANSI_down,locy+1));
				locx--;
			}
			do {
				x = getch_(0);
			} while (x != '\n');
			if (tile_flag(locx,locy,SOLID)) {
				pr_info("The magic missile hits a wall!");
			} else {
				pr_info("The magic missile hits a foe!");
				mdamages[locy][locx] += 30;
			}
			break;
		default:
			pr_info("What?");
			power += 5;
		}
	} else {
		pr_info("Never mind.");
	}
}
int conds[10];
int numconds = 0;
int condDur[10];
void applyCond(int cond, int dur) {
	conds[numconds] = cond;
	condDur[numconds] = dur;
	numconds++;
}
void spSpeed() {
	power -= 10;
	applyCond(COND_SPEED,30);
}
void spTeleport() {
	printf("%sYou teleport!\nWhich direction? (arrows)",ANSI_home);
	char x = getch_(0);
	int locx = player_x;
	int locy = player_y;
	do {
		x = getch_(0);
	} while (x != '\x1b');
	x = getch_(0);
	if (x == '\x5b') {
		power -= 10;
		switch(getch_(0)) {
		case 'A':
			locy--;
			while (!monsters[locy][locx] && !tile_flag(locx,locy,SOLID)) {
				printf("%s%s%s|",ANSI_home,times(ANSI_right,locx),times(ANSI_down,locy+1));
				locy--;
			}
			locy++;
			do {
				x = getch_(0);
			} while (x != '\n');
			player_x = locx;
			player_y = locy;
			break;
		case 'B':
			locy++;
			while (!monsters[locy][locx] && !tile_flag(locx,locy,SOLID)) {
				printf("%s%s%s|",ANSI_home,times(ANSI_right,locx),times(ANSI_down,locy+1));
				locy++;
			}
			locy--;
			do {
				x = getch_(0);
			} while (x != '\n');
			player_x = locx;
			player_y = locy;
			break;
		case 'C':
			locx++;
			while (!monsters[locy][locx] && !tile_flag(locx,locy,SOLID)) {
				printf("%s%s%s-",ANSI_home,times(ANSI_right,locx),times(ANSI_down,locy+1));
				locx++;
			}
			locx--;
			do {
				x = getch_(0);
			} while (x != '\n');
			player_x = locx;
			player_y = locy;
			break;
		case 'D':
			locx--;
			while (!monsters[locy][locx] && !tile_flag(locx,locy,SOLID)) {
				printf("%s%s%s-",ANSI_home,times(ANSI_right,locx),times(ANSI_down,locy+1));
				locx--;
			}
			do {
				x = getch_(0);
			} while (x != '\n');
			locx++;
			player_x = locx;
			player_y = locy;
			break;
		default:
			pr_info("What?");
			power += 5;
		}
	} else {
		pr_info("Never mind.");
	}
}
int hp = 100; // hit points
void spHeal() {
	power -= 10;
	hp = maxhp;
}
void (*spells[]) (void) = {0, spMagicMissile, spSpeed, spTeleport, spHeal};
void gainSpell(int id) {
	if (is_contained_in(knownSpells,numSpells,id)) {
		pr_info("You already knew that spell!");
		power += spellCosts[id];
		spells[id]();
	} else {
		char buffer[64];
		sprintf(buffer,"You learned the \"%s\" spell!",spellNames[id]);
		pr_info(buffer);
		knownSpells[numSpells++] = id;
	}
}


void randSpellEffect(int mustBad) {
	int effectNum = rand()%(mustBad ? 6 : 15);
	switch(effectNum) {
		case 0:
		pr_info("You can't see!");
		applyCond(COND_BLIND, 20);
		break;
		case 1:
		pr_info("You feel weak!");
		applyCond(COND_WEAK, 20);
		break;
		case 2:
		pr_info("You find yourself somewhere else!");
		do {
			player_x = rand()%D_WIDTH;
			player_y = rand()%D_HEIGHT;
		} while (tile_flag(player_x,player_y,SOLID));
		break;
		case 3:
		pr_info("You are struck with pain!");
		hp /= 2;
		break;
		case 4:
		pr_info("You can't move!");
		applyCond(COND_NOMOVE, 20);
		break;
		case 5:
		pr_info("You feel slower!");
		applyCond(COND_SLOW, 20);
		break;
		case 6:
		case 7:
		hp = maxhp;
		break;
		case 8:
		pr_info("You feel faster!");
		applyCond(COND_SPEED, 20);
		break;
		case 9:
		pr_info("You feel invisible!");
		applyCond(COND_NOSEE, 20);
		break;
		case 10:
		pr_info("Your strength increases!");
		playerStr++;
		break;
	}
}


void remCond(int cond) {
	int loc = is_contained_in(conds, numconds, cond)-1;
	conds[loc] = COND_NULL;
	while (conds[numconds] == COND_NULL && numconds)
	{
		numconds--;
	}
}
int attack(int strengthAtk, int strengthDef, char *monstName, bool you) {
	int sval, mod, ench;
	if (you) {
		sval = WEAPON_SS[weapon_on]-playerStr-weapon_en;
		mod = weapon_on - ((sval>0)?sval:0);
		ench = weapon_en*2;
		strengthAtk += mod+ench;
	} else {
		sval = ARMOR_SS[armor_on]-playerStr-armor_en;
		mod = armor_on - ((sval>0)?sval:0);
		ench = armor_en*2;
		strengthDef += mod+ench;
	}
	if (you && is_contained_in(conds, numconds, COND_WEAK)) {
		strengthAtk /= 3;
		strengthAtk *= 2;
	} else if (!you && is_contained_in(conds, numconds, COND_WEAK)) {
		strengthDef /= 3;
		strengthDef *= 2;
	}
	if (strengthDef < 4) {
		strengthDef = 4;
	}
	if (strengthAtk < 1) {
		strengthAtk = 1;
	}
	strengthAtk += 6;
	int missChance = (strengthAtk-((!you && is_contained_in(conds, numconds, COND_NOSEE)) ? (strengthDef*2) : strengthDef))/2;
	if (missChance < 1) missChance = 1;
	int val = rand()%missChance;
	char theName[100];
	sprintf(theName,"The %s",monstName);
	if (val) { // hit
		sprintf(theName,"%s hit%s for %d damage!",you?"You":theName,you?"":"s",val*strengthAtk/(strengthDef/4));
		pr_info(theName);
		return val*strengthAtk/(strengthDef/4);
	} else { // miss
		sprintf(theName,"%s miss%s!",you?"You":theName,you?"":"es");
		pr_info(theName);
		return 0;
	}
}
// useful function, this is
// for example:
// str_format("hello",0,1) => "hello"
// str_format("hello",1,1) => "a hello"
// str_format("ohio",1,1) => "an ohio"
// str_format("hello",2,1) => "the hello"
// str_format("hello",0,10) => "10 hellos"
// str_format("hello",1,10) => "10 hellos"
// str_format("hello",2,10) => "the 10 hellos"
// str_format("hello world",0,10) => "10 hello worlds"
// str_format("hello of world",0,10) => "10 hellos of world"
// str_format("foo bar of baz",0,10) => "10 foo bars of baz"
char *str_format(char *str_pl, int article, int quantity) {
	char str[256];
	strcpy(str,str_pl); // incase we used a literal so we cant edit
	char *buffer = (char *)malloc(256); // malloc so it doesn't go out of scope
	if (quantity == 1) {
		// singular
		// the ternary expressions are really long and hard to understand
		sprintf(buffer,"%s %s",(article?(article==2?"the":((str[0] == 'a' || str[0] == 'e' || str[0] == 'i' || str[0] == 'o' || str[0] == 'u')?"an":"a")):""),str);
	} else {
		// pluralize
		char *plural = (char *)malloc(256);
		char *of_sep = strstr(str," of ");
		strcpy(plural,"");
    if (!of_sep) { // no "of", null ptr
      strcat(plural,str);
      strcat(plural,"s");
    } else {
		  *of_sep = '\0'; // cut off
		  strcat(plural,str); // put everything before the of
		  strcat(plural,"s of "); // pluralize it
		  *of_sep = ' '; // don't overall change str
		  of_sep += 4; // rest of str
		  strcat(plural,of_sep); // append rest of string
    }
		if (quantity == 0) {
			sprintf(buffer,"%sno %s",article==2?"the ":"",plural);
		} else {
			sprintf(buffer,"%s%d %s",article==2?"the ":"",quantity,plural);
		}
	}
	return buffer;
}

int ai_data[D_HEIGHT][D_WIDTH]; // can edit stuff
void basic_monst_ai(int x, int y) {
	if (!tile_flag(x-1,y,SOLID)) {
	monsters[y][x-1] = MONSTER_BASIC;
	monsters[y][x] = 0;
	}
	if (near(x, y, player_x, player_y)) {
		hp -= attack(10, playerStr, "random thing", 0);
		death_reason = "a random thing";
	}
}
void kobold_ai(int x, int y) {
	if (near(x,y,player_x,player_y)) {
		hp -= attack(10, playerStr, "kobold", 0);
		death_reason = "a kobold";
		return;
	}
	if (!ai_data[y][x]) { // has not seen you
		if (rand()%2) {
			switch (rand()%4) {
			case 0:
				if (!tile_flag(x-1,y,SOLID)) {
					monsters[y][x-1] = KOBOLD;
					monsters[y][x] = 0;
					mdamages[y][x-1] = mdamages[y][x];
				}
				break;
			case 1:
				if (!tile_flag(x+1,y,SOLID)) {
					monsters[y][x+1] = KOBOLD;
					monsters[y][x] = 0;
					mdamages[y][x+1] = mdamages[y][x];
				}
				break;
			case 2:
				if (!tile_flag(x,y+1,SOLID)) {
					monsters[y+1][x] = KOBOLD;
					monsters[y][x] = 0;
					mdamages[y+1][x] = mdamages[y][x];
				}
				break;
			case 3:
				if (!tile_flag(x,y-1,SOLID)) {
					monsters[y-1][x] = KOBOLD;
					monsters[y][x] = 0;
					mdamages[y-1][x] = mdamages[y][x];
				}
				break;
			}
		}
		if (visarray[y][x] && !(rand()%4)) {
			pr_info("The kobold sees you!");
			ai_data[y][x] = 1;
		}
	} else {
		bool justupd[D_HEIGHT][D_WIDTH];
		for (int moves = 0; moves < 1; moves++) {
			for (int i = 0; i < D_HEIGHT; i++) {
				for (int j = 0; j < D_WIDTH; j++) {
					visarray[i][j] = 0;
					justupd[i][j] = false;
				}
			}
			visarray[player_y][player_x] = 1;
			for (int iter = 0; iter < VISION_RANGE*2-2; iter++) {
				for (int i = 0; i < D_HEIGHT; i++) {
					for (int j = 0; j < D_WIDTH; j++) {
						justupd[i][j] = false;
					}
				}
				for (int i = 0; i < D_HEIGHT; i++) {
					for (int j = 0; j < D_WIDTH; j++) {
						if (visarray[i][j] ) continue;
						if (visarray[i-1][j] && !justupd[i-1][j] && !tile_flag(j,i-1,SOLID)) {
							visarray[i][j] = visarray[i-1][j]+1;
							justupd[i][j] = true;
						} else if (visarray[i+1][j] && !justupd[i+1][j] && !tile_flag(j,i+1,SOLID)) {
							visarray[i][j] = visarray[i+1][j]+1;
							justupd[i][j] = true;
						} else if (visarray[i][j-1] && !justupd[i][j-1] && !tile_flag(j-1,i,SOLID)) {
							visarray[i][j] = visarray[i][j-1]+1;
							justupd[i][j] = true;
						} else if (visarray[i][j+1] && !justupd[i][j+1] && !tile_flag(j+1,i,SOLID)) {
							visarray[i][j] = visarray[i][j+1]+1;
							justupd[i][j] = true;
						}
					}
				}
				/*
				for (int i = 0; i < D_HEIGHT; i++) {
					for (int j = 0; j < D_WIDTH; j++) {
						printf("%d ",visarray[i][j]);
					}
					printf("\n");
				}
				getch_(0);
				*/
			}
			/*
			for (int i = 0; i < D_HEIGHT; i++) {
				for (int j = 0; j < D_WIDTH; j++) {
					if (i == y && j == x) printf("K ");
					else printf("%d ",visarray[i][j]);
				}
				printf("\n");
			}
			getch_(0);
			*/
			int poss_dx[4];
			int poss_dy[4];
			int num_poss = 0;
			if (visarray[y][x] > visarray[y-1][x]) {
				poss_dx[num_poss] = 0;
				poss_dy[num_poss] = -1;
				num_poss++;
			}
			if (visarray[y][x] > visarray[y+1][x]) {
				poss_dx[num_poss] = 0;
				poss_dy[num_poss] = 1;
				num_poss++;
			}
			if (visarray[y][x] > visarray[y][x-1]) {
				poss_dx[num_poss] = -1;
				poss_dy[num_poss] = 0;
				num_poss++;
			}
			if (visarray[y][x] > visarray[y][x+1]) {
				poss_dx[num_poss] = 1;
				poss_dy[num_poss] = 0;
				num_poss++;
			}
			if (num_poss) {
				int choice = rand()%num_poss;
				int dx = poss_dx[choice];
				int dy = poss_dy[choice];
				if (visarray[y+dy][x+dx] == 1) {
					hp -= attack(9, playerStr, "kobold", 0);
					death_reason = "a kobold";
			    } else if (!tile_flag(x+dx,y+dy,SOLID)) {
					monsters[y+dy][x+dx] = KOBOLD;
					monsters[y][x] = 0;
					mdamages[y+dy][x+dx] = mdamages[y][x];
				}
			}
		}
	}
}
// monster ais
void (*MONST_AIS[])(int x, int y) = {basic_monst_ai,basic_monst_ai,kobold_ai};
bool speed_toggle;
int main() {
	FILE *sfile = fopen("seed","w");
	int curseed;
  printf("Generate random dungeon? (y/n) ");
  if (getch_(1) == 'y') {
    curseed = time(NULL);
  } else {
    printf("\nEnter seed: ");
    scanf("%d",&curseed);
  }
	fprintf(sfile,"%d",curseed);
	fclose(sfile);
	srand(curseed);
	shuffle_rand_items();
	display_generic_specific();
	printf("%s",ANSI_clr);
	printc("Welcome to YACrogue!\nMade by Vijay Shanmugam and Joshua Piety\nCollect the mighty Omelette of Yendor from the 26th floor of the dungeon!\nPress ? for help at any time.\nHave fun! (press a key to start)",GREEN);
	getch_(0);
	printc("\nWould you like to view help? ",GREEN);
	if (getch_(1) == 'y') {
		printf("%s%sWelcome to YACROGUE! In this game, you must adventure through the dungeon! Note that this is a VERY early version of the game.\nCommands:\narrows - move, or attack a monster by moving into it\ni - view your items\nw - view armor and weapons\nu - use an item\np - pick up an item on the floor\n? - view help\nshift-d - activate DEBUG MODE (if you are testing this game, this mode is highly suggested!)\nPress any key to go to next page...\n",ANSI_clr,ANSI_home);
		getch_(0);
		printf("%s%sPotions are magical elixirs that one can drink to have varying effects. Some do nothing, some have positive effects and some have negative effects. In this early version, some potions cannot be used. \nYou will not immediately know what a potion does upon finding it always. Some potions will simply be identified by a color. Negative potions are always found disguised as positive potions. \nNOTE: It is RANDOM what colors correspond to what potions. DO NOT ASK ME \"what the red potion does\". I DO NOT KNOW.\nThe following potions exist:\nHealing - heals you.\nAcid - hurts you.\nSpeed - speeds your movement and attack.\nSlow - slows your movement and attack.\nStrength - permanently increases your strength.\nWeakness - decreases your strength for some time.\nInvisibility - makes you invisible.\nBlindness - causes you to go blind temporarily.\nEndurance - increases your maximum health.\nPress any key to go to the next page...\n",ANSI_clr,ANSI_home);
		getch_(0);
		printf("%s%sYou will encounter monsters in the dungeon. Only one currently exists: the kobold. It will attempt to move towards you and attack when it is near. Defeat them by any means!\nLava also exists in the dungeon. If you touch it, you die.\nPress any key to continue...",ANSI_clr,ANSI_home);
		getch_(0);
		printf("%s%sArmor (leather armor) and weapons (daggers) can be found around the dungeon. They will increase your combat capabilities, but beware - some can be found cursed which means that not only do they have a negative effect, but they cannot be removed.\nYou can also find scrolls of enchantment, which will increase the power of a piece of equipment, and can remove curses. They can also be used on potions to turn a negative potion into a neutral potion and a neutral potion into a positive potion. However, in order to use them on armor or weapons they must be in your inventory rather than equipped - use \"r\" to remove weapons and armor.\nPress any key to exit help...",ANSI_clr,ANSI_home);
		getch_(0);
		printf("%s%sSpellbooks are magical tomes that can be found occasionally around the dungeon. They imbue you with the magical energy necessary to cast a spell. This can be done by pressing \"c\" to cast. Spells require magic points (MP) to cast.\nWhen a spellbook is read, it can be misinterpreted to produce a random effect. Beware: if it is cursed, that will happen 100%% of the time, and always with a bad effect!\nSometimes, you start the game with a spell. If you read a spellbook of a spell you already know, it will give you a free casting of that spell (without expending MP).\nPress any key to exit help...",ANSI_clr,ANSI_home);
		getch_(0);
	}
	fill_tiles(0,0,D_WIDTH,D_HEIGHT,T_AIR);
	for (int i = 0; i < D_HEIGHT; i++) {
		for (int j = 0; j < D_WIDTH; j++) {
			seen[i][j] = -1;
		}
	}
	initPlants();
	printf("%s\n",T_NAMES[6]);
	system("python3 dungeon.py"); // yes, i know, this is a dumb way to do it
	FILE *dungFile = fopen("dungeon.out","r");
	for (int i = 0; i < D_HEIGHT; i++) {
		for (int j = 0; j < D_WIDTH; j++) {
			fscanf(dungFile,"%d",&(dungeon[i][j]));
			fgetc(dungFile);
			if (dungeon[i][j] == T_UPS) {
				player_x = j;
				player_y = i;
			}
		}
	}
	// no monsters will spawn here
	visarray[player_y][player_x] = 1;
	for (int iter = 0; iter < VISION_RANGE; iter++) {
		for (int i = 0; i < D_HEIGHT; i++) {
			for (int j = 0; j < D_WIDTH; j++) {
				if (visarray[i][j] != 1 && ((visarray[i+1][j] == 1 && !tile_flag(j,i+1,SOLID)) || (visarray[i-1][j] == 1 && !tile_flag(j,i-1,SOLID)) || (visarray[i][j+1] == 1 && !tile_flag(j+1,i,SOLID))|| (visarray[i][j-1] == 1 && !tile_flag(j-1,i,SOLID)))) {
					visarray[i][j] = 2;
				}
			}
		}
		for (int i = 0; i < D_HEIGHT; i++) {
			for (int j = 0; j < D_WIDTH; j++) {
				if (visarray[i][j] == 2) {
					visarray[i][j] = 1;
				}
			}
		}
	}
	// gen monsters
	for (int i = 0; i < D_HEIGHT; i++) {
		for (int j = 0; j < D_WIDTH; j++) {
			mdamages[i][j] = 0;
			if (!dungeon[i][j] && !visarray[i][j] && !(rand()%40)) {
				monsters[i][j] = KOBOLD;
			}
		}
	}
	// gen items
	for (int i = 0; i < D_HEIGHT; i++) {
		for (int j = 0; j < D_WIDTH; j++) {
			if (!dungeon[i][j] && !(rand()%30)) {
				items[i][j] = (rand()%MAXITEM)+1;
				enchs[i][j] = (rand()%3)-1;
			} else if (!dungeon[i][j] && !(rand()%120)) {
				items[i][j] = I_POTION_HEAL;
				enchs[i][j] = 1;
			}
		}
	}
	// placeholder for dungeon gen
	/*
	player_x = 1;
	player_y = 1;
	dungeon[6][6] = T_LAVA;
	items[7][8] = I_POTION_GREEN;
	items[8][8] = I_POTION_RED;
	items[9][8] = I_POTION_SPEED;
	items[10][9] = I_POTION_INVIS;
	enchs[9][8] = 2;
	enchs[10][9] = 2;
	monsters[8][9] = MONSTER_BASIC;
	learn(I_POTION_RED);
	*/
	bool game_running = true;
	pr_info("Welcome to INROGUE!");
	if (rand()%3) {
		gainSpell(rand()%4+1);
	}
	int dlevel = 0;
	while (game_running) {
		enforce_borders();
		printf("%s",ANSI_clr);
		// display health bar
		for (int c = 0; c < numconds; c++) {
			if (!(condDur[c]--)) {
				remCond(conds[c]);
			}
		}
		int num_dash = 10*((double)hp/(double)maxhp);
		int barcol;
		switch (num_dash) {
		case 0:
		case 1:
		case 2:
			barcol = MAGENTA;
			break;
		case 3:
		case 4:
		case 5:
			barcol = RED;
			break;
		case 6:
		case 7:
		case 8:
			barcol = YELLOW;
			break;
		case 9:
		case 10:
			barcol = GREEN;
		}
		printf("%s", ANSI_home);
		printf("HP: |");
		color_set(barcol);
		printf("%s%s",times("-",num_dash),times(" ",10-num_dash));
		color_reset();
		printf("| ");
		if (rand()%20 == 0 && power < maxpow) power++;
		printf("MP: %d ",power);
		for (int c = 0; c < numconds; c++) {
			switch (conds[c]) {
			 case COND_SPEED:
				 color_set(GREEN);
				 printf("SPEED ");
				 break;
			 case COND_SLOW:
				 color_set(RED);
				 printf("SLOW ");
				 break;
			 case COND_NOSEE:
				 color_set(GREEN);
				 printf("INVIS ");
				 break;
			 case COND_WEAK:
				 color_set(RED);
				 printf("WEAK ");
				 break;
			 case COND_BLIND:
				 color_set(RED);
				 printf("BLIND ");
				 break;
			 case COND_NOMOVE:
			 	 color_set(RED);
			 	 printf("PARA ");
			 	 break;
			}
		}
		color_reset();
		printf("\n");
		// calculate vision
		for (int i = 0; i < D_HEIGHT; i++) {
			for (int j = 0; j < D_WIDTH; j++) {
				visarray[i][j] = 0;
			}
		}
		visarray[player_y][player_x] = 1;
		for (int iter = 0; iter < VISION_RANGE && !is_contained_in(conds, numconds, COND_BLIND); iter++) {
			for (int i = 0; i < D_HEIGHT; i++) {
				for (int j = 0; j < D_WIDTH; j++) {
					if (visarray[i][j] != 1 && ((visarray[i+1][j] == 1 && !tile_flag(j,i+1,SOLID)) || (visarray[i-1][j] == 1 && !tile_flag(j,i-1,SOLID)) || (visarray[i][j+1] == 1 && !tile_flag(j+1,i,SOLID))|| (visarray[i][j-1] == 1 && !tile_flag(j-1,i,SOLID)))) {
						visarray[i][j] = 2;
					}
				}
			}
			for (int i = 0; i < D_HEIGHT; i++) {
				for (int j = 0; j < D_WIDTH; j++) {
					if (visarray[i][j] == 2) {
						visarray[i][j] = 1;
					}
				}
			}
		}
		for (int i = 0; i < D_HEIGHT; i++) {
			for (int j = 0; j < D_WIDTH; j++) {
				if (visarray[i][j] || OMNISCIENCE) {
					see(j,i);
				}
			}
		}
		display_dungeon(0,0);
		display_items(items);
		display_monsts(monsters);
		display_player(); // display player AFTER items, so it shows as @ when you're on an item
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
		} else if (keypress == 'p') { // pick up items
			if (items[player_y][player_x]) {
				add_item(items[player_y][player_x],1,enchs[player_y][player_x]);
				items[player_y][player_x] = I_NOTHING;
			} else {
				pr_info("You see nothing to pick up.");
			}
		} else if (keypress == 'r') { // remove armor and weapons
			if (armor_on) {
				if (armor_en < 0) pr_info("You can't remove the cursed armor you're wearing..."); else {

				add_item(I_ARMOR_1-1+armor_on,1,armor_en);
				armor_on = 0;
				armor_en = 0;
				}
			}
			if (weapon_on) {
				if (weapon_en < 0) pr_info("You can't put down the cursed weapon you're wielding..."); else {
				add_item(I_WEAPON_1-1+weapon_on,1,weapon_en);
				weapon_on = 0;
				weapon_en = 0;
				}
			}
		} else if (keypress == 'u') { // use items
			take_inventory(1);
			printf("%sWhich item number? ",ANSI_home);
			int inum;
			scanf("%d",&inum);
			if (inum > num_pack_slots || inum < 1) {
				pr_info("You don't have that many items.");
			} else {
				inum--;
				int item = pack_items[inum];
				if (item >= FIRST_GENERICS[0] && item < FIRST_GENERICS[0]+NUM_OF_IDEN_ITEMS) { // it is a generic
					item = iden_specifics[is_contained_in(iden_generics,NUM_OF_IDEN_ITEMS,item)-1]; // make it a specific just this once
				}
				if (item == I_POTION_HEAL) {
					if (pack_enchs[inum] == 0) { // uncursed
						pr_info("You feel... nothing.");
					} else if (pack_enchs[inum] > 0) {
						pr_info("You feel healthier!");
						hp = maxhp;
						learn(pack_items[inum]);
					} else {
						pr_info("Acid spills from the bottle!");
						hp /= 2;
						learn(pack_items[inum]);
					}
					del_item(pack_items[inum],1);
				} else if (item == I_POTION_SPEED) {
					if (pack_enchs[inum] == 0) { // uncursed
						pr_info("You feel... nothing.");
					} else if (pack_enchs[inum] > 0) {
						pr_info("You feel faster!");
						applyCond(COND_SPEED,10);
						learn(pack_items[inum]);
					} else {
						pr_info("You feel slower...");
						applyCond(COND_SLOW,10);
						learn(pack_items[inum]);
					}
					del_item(pack_items[inum],1);
				} else if (item == I_POTION_INVIS) {
					if (pack_enchs[inum] == 0) { // uncursed
						pr_info("You feel... nothing.");
					} else if (pack_enchs[inum] > 0) {
						pr_info("You feel invisible!");
						applyCond(COND_NOSEE,20);
						learn(pack_items[inum]);
					} else {
						pr_info("You feel... something. You just can't remember what..."); // amnesia
						num_known--;
						learn(pack_items[inum]);
					}
					del_item(pack_items[inum],1);
				} else if (item == I_POTION_STRENGTH) {
					if (pack_enchs[inum] == 0) { // uncursed
						pr_info("You feel... nothing.");
					} else if (pack_enchs[inum] > 0) {
						pr_info("You feel stronger!");
						playerStr++;
						learn(pack_items[inum]);
					} else {
						pr_info("You feel weaker...");
						applyCond(COND_WEAK,20);
						learn(pack_items[inum]);
					}
					del_item(pack_items[inum],1);
				} else if (item == I_POTION_MAXHP) {
					if (pack_enchs[inum] == 0) { // uncursed
						pr_info("You feel... nothing.");
					} else if (pack_enchs[inum] > 0) {
						pr_info("Your maximum HP increases!");
						hp += maxhp/3;
						maxhp += maxhp/3;
						learn(pack_items[inum]);
					} else {
						pr_info("You can't see!");
						applyCond(COND_BLIND,20);
						learn(pack_items[inum]);
					}
					del_item(pack_items[inum],1);
				} else if (item > I_POTION_MAXHP && item < I_SCROLL_ENCH) {
					pr_info("You drink the potion, but nothing happens.");
					del_item(pack_items[inum],1);
				} else if (item >= I_SPELLBOOK_MM && item <= I_SPELLBOOK_HP) {
					int spellType = item - I_SPELLBOOK_MM + 1;
					if (pack_enchs[inum] == 0) {
						if (rand()%3 == 0) {
							gainSpell(spellType);
						} else {
							if (rand()%3 == 0) {
								pr_info("You read the spell wrong!");
								randSpellEffect(0);
							} else {
								pr_info("You can't read this spellbook; it's too complicated.");							}
						}
					} else if (pack_enchs[inum] > 0) {
						gainSpell(spellType);
					} else {
						pr_info("This spellbook was cursed!");
						randSpellEffect(1);
					}
					del_item(pack_items[inum],1);
				} else if (item >= I_ARMOR_1 && item < I_WEAPON_1) {
					if (armor_on) {
						if (armor_en < 0) pr_info("You can't remove the cursed armor you're wearing..."); else
						add_item(I_ARMOR_1-1+armor_on,1,armor_en);
					}
					armor_on = pack_items[inum]-I_ARMOR_1+1;
					armor_en = pack_enchs[inum];
					del_item(pack_items[inum],1);
				} else if (item >= I_WEAPON_1 && item < I_FOOD) {
					if (weapon_on) {
						if (weapon_en < 0) pr_info("You can't put down the cursed weapon you're wielding..."); else
						add_item(I_WEAPON_1-1+weapon_on,1,weapon_en);
					}
					weapon_on = pack_items[inum]-I_WEAPON_1+1;
					weapon_en = pack_enchs[inum];
					del_item(pack_items[inum],1);
				} else if (item == I_SCROLL_ENCH) {
					pr_info("You recite the words on this old scroll...");
					take_inventory(1);
					int scrench = pack_enchs[inum];
					del_item(pack_items[inum],1);
					printf("%sThis is a scroll of enchantment. What to enchant?",ANSI_home);
					scanf("%d",&inum);
					inum--;
					if (scrench < 0) {
						pr_info("The scroll, being cursed, disintegrates in your hands and has no effect.");
					} else {
						pack_enchs[inum]++;
						if (!pack_enchs[inum]) {
							pr_info("You feel an evil energy dissipate with the use of this scroll.");
						}
						if (scrench) {
							pr_info("The scroll, being blessed, adds its magical energy to the item being enchanted.");
							pack_enchs[inum]++;
							if (!pack_enchs[inum]) {
								pr_info("You feel an evil energy dissipate with the use of this scroll.");
							}
						}
					}
				} else {
					char buffer[255];
					sprintf(buffer,"%s? You can't use that!",str_format(ITEM_NAMES[pack_items[inum]],1,pack_counts[inum])); // use pack_items[inum] to avoid disclosing information
					pr_info(buffer);
				}
			}
		} else if (keypress == 'w') { // view wielding and wearing
			printf("%sWearing:\n%s%s\nWielding:\n%s%s\n",ANSI_home,armor_on?ITEM_NAMES[I_ARMOR_1+armor_on-1]:"nothing",(playerStr>=ARMOR_SS[armor_on])?"":" (heavy)",weapon_on?ITEM_NAMES[I_WEAPON_1+weapon_on-1]:"nothing",(playerStr>=WEAPON_SS[weapon_on])?"":" (heavy)");
			getch_(0);
		} else if (keypress == 'i') { // take inventory
			take_inventory(0);
			printf("(press any key)");
			getch_(0);
		} else if (keypress == '\x1b') { // appears to be an ANSI escape sequence
			keypress = getch_(0);
			if (keypress == '\x5b') { // which it is!
				keypress = getch_(0); // meat of the escape sequence
				switch (keypress) {
					case 'A': // up arrow
					if (is_contained_in(conds,numconds,COND_NOMOVE))
						pr_info("You can't move!");
					else if (monsters[player_y-1][player_x])
						mdamages[player_y-1][player_x] += attack(playerStr, MONST_STRS[monsters[player_y-1][player_x]], "YOU SHOULD NEVER SEE THIS. IF YOU DO THERE IS A GLITCH IN THE GAME",1);
					else if (!tile_flag(player_x, player_y - 1, SOLID))
						player_y--;
					break;
					case 'B': // down arrow
					if (is_contained_in(conds,numconds,COND_NOMOVE))
						pr_info("You can't move!");
					else if (monsters[player_y+1][player_x])
						mdamages[player_y+1][player_x] += attack(playerStr, MONST_STRS[monsters[player_y+1][player_x]], "YOU SHOULD NEVER SEE THIS. IF YOU DO THERE IS A GLITCH IN THE GAME",1);
					else if (!tile_flag(player_x, player_y + 1, SOLID))
						player_y++;
					break;
					case 'C': // right arrow
					if (is_contained_in(conds,numconds,COND_NOMOVE))
						pr_info("You can't move!");
					else if (monsters[player_y][player_x+1])
						mdamages[player_y][player_x+1] += attack(playerStr, MONST_STRS[monsters[player_y][player_x+1]], "YOU SHOULD NEVER SEE THIS. IF YOU DO THERE IS A GLITCH IN THE GAME",1);
					else if (!tile_flag(player_x + 1, player_y, SOLID))
						player_x++;
					break;
					case 'D': // left arrow
					if (is_contained_in(conds,numconds,COND_NOMOVE))
						pr_info("You can't move!");
					else if (monsters[player_y][player_x-1])
						mdamages[player_y][player_x-1] += attack(playerStr, MONST_STRS[monsters[player_y][player_x-1]], "YOU SHOULD NEVER SEE THIS. IF YOU DO THERE IS A GLITCH IN THE GAME",1);
					else if (!tile_flag(player_x - 1, player_y, SOLID))
						player_x--;
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
		} else if (keypress == '?') {
			printf("%s%sWelcome to YACROGUE! In this game, you must adventure through the dungeon! Note that this is a VERY early version of the game.\nCommands:\narrows - move, or attack a monster by moving into it\ni - view your items\nw - view armor and weapons\nu - use an item\np - pick up an item on the floor\n? - view help\nshift-d - activate DEBUG MODE (if you are testing this game, this mode is highly suggested!)\nPress any key to go to next page...\n",ANSI_clr,ANSI_home);
			getch_(0);
			printf("%s%sPotions are magical elixirs that one can drink to have varying effects. Some do nothing, some have positive effects and some have negative effects. In this early version, some potions cannot be used. \nYou will not immediately know what a potion does upon finding it always. Some potions will simply be identified by a color. Negative potions are always found disguised as positive potions. \nNOTE: It is RANDOM what colors correspond to what potions. DO NOT ASK ME \"what the red potion does\". I DO NOT KNOW.\nThe following potions exist:\nHealing - heals you.\nAcid - hurts you.\nSpeed - speeds your movement and attack.\nSlow - slows your movement and attack.\nStrength - permanently increases your strength.\nWeakness - decreases your strength for some time.\nInvisibility - makes you invisible.\nBlindness - causes you to go blind temporarily.\nEndurance - increases your maximum health.\nPress any key to go to the next page...\n",ANSI_clr,ANSI_home);
			getch_(0);
			printf("%s%sYou will encounter monsters in the dungeon. Only one currently exists: the kobold. It will attempt to move towards you and attack when it is near. Defeat them by any means!\nLava also exists in the dungeon. If you touch it, you die.\nPress any key to continue...",ANSI_clr,ANSI_home);
			getch_(0);
			printf("%s%sArmor (leather armor) and weapons (daggers) can be found around the dungeon. They will increase your combat capabilities, but beware - some can be found cursed which means that not only do they have a negative effect, but they cannot be removed.\nYou can also find scrolls of enchantment, which will increase the power of a piece of equipment, and can remove curses. They can also be used on potions to turn a negative potion into a neutral potion and a neutral potion into a positive potion. However, in order to use them on armor or weapons they must be in your inventory rather than equipped - use \"r\" to remove weapons and armor.\nPress any key to continue...",ANSI_clr,ANSI_home);
			getch_(0);
			printf("%s%sSpellbooks are magical tomes that can be found occasionally around the dungeon. They imbue you with the magical energy necessary to cast a spell. This can be done by pressing \"c\" to cast. Spells require magic points (MP) to cast.\nWhen a spellbook is read, it can be misinterpreted to produce a random effect. Beware: if it is cursed, that will happen 100%% of the time, and always with a bad effect!\nSometimes, you start the game with a spell. If you read a spellbook of a spell you already know, it will give you a free casting of that spell (without expending MP).\nPress any key to exit help...",ANSI_clr,ANSI_home);
			getch_(0);
		} else if (keypress == 'D') {
			printf("%s\n1. Give item\n2. Identify item\n3. Give condition\n4. Exit",ANSI_home);
			printf("%sSelect choice: ",ANSI_home);
			int dchoice;
			scanf("%d",&dchoice);
			if (dchoice == 1) {
				printf("%s%s\n",ANSI_home,ANSI_clr);
				for (int i = 1; i <= MAXITEM; i++) {
					printf("%d. %s\n",i,ITEM_NAMES[i]);
				}
				int qchoice, echoice;
				printf("%sSelect choice: ",ANSI_home);
				scanf("%d",&dchoice);
				printf("%sSelect quantity: ",ANSI_home);
				scanf("%d",&qchoice);
				printf("%sSelect enchantment (negative = bad, positive = good, zero = not magical): ",ANSI_home);
				scanf("%d",&echoice);
				add_item(dchoice,qchoice,echoice);
			} else if (dchoice == 2) {
				printf("%s",ANSI_clr);
				take_inventory(1);
				printf("%sWhich item number? ",ANSI_home);
				scanf("%d",&dchoice);
				learn(pack_items[dchoice-1]);
			}
		} else if (keypress == '$') {
			printf("%sCurrent depth: %d\nCurrent seed: %d\n",ANSI_home,dlevel,curseed);
			getch_(0);
		} else if (keypress == 'c') { // cast a spell!
			if (numSpells) {
				printf("%s\n",ANSI_home);
				int validSpell = 1;
				for (int i = 0; i < numSpells; i++) {
					if (spellCosts[knownSpells[i]] <= power) {
						printf("%d. %s (%d MP)\n",validSpell,spellNames[knownSpells[i]],spellCosts[knownSpells[i]]);
						validSpell++;
					} else {
						faint(1);
						printf("    %s (%d MP)\n",spellNames[knownSpells[i]],spellCosts[knownSpells[i]]);
						faint(0);
					}
				}
				printf("%sSelect spell:",ANSI_home);
				int spellChoice;
				scanf("%d",&spellChoice);
				if (spellChoice < 0 || spellChoice >= validSpell) {
					pr_info("You don't have that many usable spells.");
				} else {
					spells[knownSpells[spellChoice-1]]();
				}
			} else {
				pr_info("You know no spells.");
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
			} else if (tile_on >= T_LASTPH) {
				char buf[64];
				sprintf(buf,"You walk through a field of %s.",T_NAMES[tile_on]);
				pr_info(buf);
			} else if (tile_on == T_DOWNS) {
				dlevel++;
				char buf[32];
				sprintf(buf,"You descend to level %d.",dlevel+1);
				pr_info(buf);
				curseed++;
				for (int i = 0; i < D_HEIGHT; i++) {
					for (int j = 0; j < D_WIDTH; j++) {
						seen[i][j] = -1;
					}
				}
				FILE *sfile = fopen("seed","w");
				fprintf(sfile,"%d",curseed);
				fclose(sfile);
				srand(curseed);
				system("python dungeon.py"); // yes, i know, this is a dumb way to do it
				FILE *dungFile = fopen("dungeon.out","r");
				for (int i = 0; i < D_HEIGHT; i++) {
					for (int j = 0; j < D_WIDTH; j++) {
						fscanf(dungFile,"%d",&(dungeon[i][j]));
						fgetc(dungFile);
						if (dungeon[i][j] == T_UPS) {
							player_x = j;
							player_y = i;
						}
					}
				}
				// no monsters will spawn here
				visarray[player_y][player_x] = 1;
				for (int iter = 0; iter < VISION_RANGE; iter++) {
					for (int i = 0; i < D_HEIGHT; i++) {
						for (int j = 0; j < D_WIDTH; j++) {
							if (visarray[i][j] != 1 && ((visarray[i+1][j] == 1 && !tile_flag(j,i+1,SOLID)) || (visarray[i-1][j] == 1 && !tile_flag(j,i-1,SOLID)) || (visarray[i][j+1] == 1 && !tile_flag(j+1,i,SOLID))|| (visarray[i][j-1] == 1 && !tile_flag(j-1,i,SOLID)))) {
								visarray[i][j] = 2;
							}
						}
					}
					for (int i = 0; i < D_HEIGHT; i++) {
						for (int j = 0; j < D_WIDTH; j++) {
							if (visarray[i][j] == 2) {
								visarray[i][j] = 1;
							}
						}
					}
				}
				// gen monsters
				for (int i = 0; i < D_HEIGHT; i++) {
					for (int j = 0; j < D_WIDTH; j++) {
						mdamages[i][j] = 0;
						if (!dungeon[i][j] && !visarray[i][j] && !(rand()%(40-dlevel*3))) {
							monsters[i][j] = KOBOLD;
						}
					}
				}
				// gen items
				for (int i = 0; i < D_HEIGHT; i++) {
					for (int j = 0; j < D_WIDTH; j++) {
						if (!dungeon[i][j] && !(rand()%30)) {
							items[i][j] = (rand()%MAXITEM)+1;
							enchs[i][j] = (rand()%3)-1;
						} else if (!dungeon[i][j] && !(rand()%(120+dlevel*10))) {
							items[i][j] = I_POTION_HEAL;
							enchs[i][j] = 1;
						}
					}
				}
			}
		}	

		// check if there's an item below you
		if (items[player_y][player_x]) {
			char *iname = ITEM_NAMES[items[player_y][player_x]];
			char tmp_s[50];
			sprintf(tmp_s,"There is %s below you.",str_format(iname,1,1));
			pr_info(tmp_s);
		}
		// check if you've died
		if (death == 3 ) {
			game_running = false;
		} else if (death == 2) {
			death = 3;
			pr_info("You died.");
			important_msg = true;
		} else if (death == 1 || hp <= 0) { // death is for abnormal deaths
			death = 2;
		}
		// update monster ais
		speed_toggle = !speed_toggle;
		if (speed_toggle || !is_contained_in(conds, numconds, COND_SPEED)) {
			for (int i = 0; (is_contained_in(conds, numconds, COND_SLOW) ? i-1 : i)<=0; i++) {
				for (int i = 0; i < D_HEIGHT; i++) {
					for (int j = 0; j < D_WIDTH; j++) {
						int x = monsters[i][j];
						if (x) {
							(*MONST_AIS[x])(j,i); // function pointers are weird
						}
					}
				}
			}
		}
	}
	if (death) { // did you die, or did the user quit?
		printf("%s",ANSI_clr);
		color_set(RED);
		printf("GAME OVER\nYou died on level %d of the dungeon.\nYou were killed by %s.\nPress any key to exit the game\n",dlevel+1,death_reason);
		color_reset();
		getch_(0);
	}
}


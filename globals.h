#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <stdint.h>

#include "room.h"
#include "curmon.h"

// This is my Atari LIBC, not a fancy one
#include <libc.h>

// The iX() and oX() mechanism
extern int (*inverb[])(int pc);
extern int (*outverb[])(int pc);

extern int16_t handle;
extern MFDB psrc, pdes;

// Game data filenames
extern char *savname[];
extern char fname[];

extern int16_t oldpal[16];
extern int16_t newpal[16];

// Logical screen address
extern uint16_t *addr;

extern char pname[20];

typedef struct {
  uint8_t chance;
  uint8_t damage;
  uint8_t weight;
} weapon_t;

extern weapon_t weapon[];
extern char wepmatx[4][4];
extern char spelinfo[24][3];

extern int16_t pxy[8];
extern int16_t bxy[4];
extern int16_t lxy[4];
extern int16_t vxy[4];

extern uint8_t curmon[12][60];

typedef struct {
  // always "1" for monsters that are defined in the table and "0" for entries that are blank.
  uint8_t in_use;
  uint8_t damage;
  uint8_t max_health;
  uint8_t monster_name_id;
  uint8_t sprite_id;
  uint8_t unknown5;
  uint8_t unknown6;
  uint8_t unknown7;
  uint8_t unknown8;
  uint8_t unknown9;
  uint8_t unknown10;
  uint8_t unknown11;
  uint8_t unknown12;
  uint8_t unknown13;
  uint8_t spell_units;
  // 15
  uint8_t physical_strength;
  // 16
  uint8_t poison_mode;
  // 17-19
  uint8_t base_chance_1_attack;
  uint8_t base_chance_2_attack;
  uint8_t base_chance_3_attack;
  // 20
  uint8_t intell_routine;
  // 21
  uint8_t bane;
  uint8_t magic_mode;
} permon_t;

extern permon_t permon[];

typedef struct {
  uint8_t object;
  uint8_t min_price;
  uint8_t unknown;
  int8_t attitude;
} sellist_t;

extern sellist_t selllist[];

// These are all to do with TOS/GEM
extern int16_t contrl[12], intin[128], intout[128], ptsin[128], ptsout[128];
extern int16_t work_in[11], work_out[57];

// buffers probably
extern char monbuf[320], junk[20];
extern int16_t crudbuf[65];
extern uint8_t putbuf[320];

typedef struct {
  // 0
  uint8_t unused0; // never used anywhere
  // 1 - 19
  uint8_t unknown1[19]; // copy of "invtrig" flags - this array overflows
  // 20
  uint8_t already_slept; // Already slept since last leaving Kilkaney
  // 21
  uint8_t dismax; // Object number above which objects can't be destroyed or disarmed
  // 22
  uint8_t player_x;
  // 23
  uint8_t player_y;
  // 24
  uint8_t winroom;
  // 25
  uint8_t current_room;
  // 26
  uint8_t outside;
  // 27
  uint8_t dungeon;
  // 28
  uint8_t lev; // "lev"
  // 29
  uint8_t new; // Never read anywhere
  // 30
  uint8_t ectoplasmic_converter_exploded;
  
  // 31-33
  // These are sometimes "dirty" flags for dungeons and sometimes object IDs for duplicator runes
  uint8_t dirty_dungeon[3];

  // 34
  uint8_t unknown34; // police flag
  // 35
  uint8_t wallfire_turns;
  // 36
  uint8_t wallfire_xy;
  // 37
  uint8_t fromout;
  // 38
  uint8_t unknown38;
  // 39
  uint8_t unknown39;
} specbuf_t;

extern specbuf_t specbuf;

typedef struct {
  uint8_t unknown0;
  // 1: A crumobj[] index if a visible object is present
  uint8_t visible;
  // 2: A curmon[] index
  uint8_t monster;
  
  uint8_t invisible;
  uint8_t unknown4;
  uint8_t unknown5; // "torch overlay number"
  uint8_t unknown6;
} zline_t;

extern zline_t zline[16][8];

extern room_data_t new_rumdata[80];
//extern uint8_t (*new_rumdata_as_array)[80][sizeof(room_data_t)];
//extern uint8_t rumdata[80][157];

extern room_zero_data_t room_zero;

extern uint8_t triglist[25];
extern char invtrig[20];

extern room_object_t crumobj[19];
extern uint8_t invnpc[4][20];
extern int pursuit[3];
extern int mode, dismax;

// Current room number (index into rumdata[])
#define ROOM_ID_LEVETHS_HOUSE 29
extern int crum;

// Each bitmap is 130 bytes. That's 128 bytes for the sprite plus one extra word, probably unused
typedef struct { uint16_t data[65]; } sprite_t;

// Objects and characters
extern sprite_t bitmap[200];
// Backgrounds
extern sprite_t fillpic[41];

extern int fromout, new, lev;
extern int monster, hold;
extern int storbuf[12][130], num, time;

extern int row, col;
extern int usedline, prev, prevy;
extern int grflist[13];
extern int winroom;

// Save game path
extern char path[64];
extern char *filename;

extern int outside, dungeon;

// If wanted by the police
extern int police;

extern int winker, winktime, combat;

extern int firebuff[130];
extern char weight[41];
extern char eats[];
extern uint8_t trigval[80][6];

// Magic
extern char spelunit[];

// Words
extern char *posture[];
extern char *wordmod[];
extern char *statword[];
extern char *wepname[];
extern char *spell[];
extern char *name[];

enum action_t {
  ACTION_NOOP,
  ACTION_ATTACK,
  ACTION_CAST,
  ACTION_EXAMINE,
  ACTION_MOVE,
  ACTION_DRAG_EAT,
  ACTION_SEARCH,
  ACTION_TAKE_DROP,
  ACTION_WAIT,
  ACTION_EXIT,
  ACTION_ROOK_SNOOP,
  ACTION_FIRE_ARROW,
  ACTION_BURN_SNUFF,
  ACTION_GIVE,
  ACTION_VOICE,
};

#define ERROR_PATH_BLOCKED 1
#define ERROR_SAME_SQUARE 2
#define ERROR_NO_LINE_OF_SIGHT 3
#define ERROR_NOTHING_TO_EXAMINE 4
#define ERROR_OBJECT_NOT_ADJACENT 5
#define ERROR_NOTHING_TO_TAKE 6
#define ERROR_INVENTORY_TOO_FULL 7
#define ERROR_NO_CHARACTER 8
#define ERROR_TARGET_NOT_ADJACENT 9
#define ERROR_LOCATION_NOT_ADJACENT 11
#define ERROR_OBJECT_IS_CLOSED 12
#define ERROR_INSUFFICIENT_SPELL_UNITS 13
#define ERROR_NO_EXIT 15
#define ERROR_TAKE_OBJECT 16
#define ERROR_ABORT_QUIT_FILE 17
#define ERROR_READ_WRITE_FAILURE 18
#define ERROR_STOP_SAVE_LOAD 19
#define ERROR_NO_WEAPON_IN_HAND 20
#define ERROR_BOW_TWO_FREE_HANDS 22
#define ERROR_NO_OBJECT 23
#define ERROR_IMMOVABLE_OBJECT 24
#define ERROR_NOTHING_TO_ROOK_SNOOP 25
#define ERROR_NOTHING_IN_HAND 27
#define ERROR_INSUFFICIENT_MAGIC_ABILITY 28
#define ERROR_HANDS_FULL 29
#define ERROR_NO_BOW_OR_ARROWS 30
#define ERROR_WHICH_HANDS_OBJECT 31
#define ERROR_DISK_SPACE 32
#define ERROR_RESTORE_MISMATCH 33

#define SPELL_CURE_LIGHT_WOUNDS 1
#define SPELL_SEARCH 2
#define SPELL_FREEZE 3
#define SPELL_UNVENOM 4
#define SPELL_VORPAL 5
#define SPELL_SPEED 6
#define SPELL_ARMOR 7
#define SPELL_DISARM 8
#define SPELL_BOLT 9
#define SPELL_UNLOCK 10
#define SPELL_FEAR 11
#define SPELL_FIREBALL 12
#define SPELL_BLINK 13
#define SPELL_REVEAL 14
#define SPELL_HEAL 15
#define SPELL_PRISM 16
#define SPELL_BLOCK 17
#define SPELL_CHARM 18
#define SPELL_FIREWALL 19
#define SPELL_DESTROY 20
#define SPELL_SUMMON 21
#define SPELL_DEATH 22
#define SPELL_LIFE 23
#define SPELL_TELEPORT 24

#define TARGET_PLACE 1
#define TARGET_OBJECT 2
#define TARGET_PLAYER 3

#define MONSTER_MELKTHROP 16

#define INTELLIGENCE_FRIEND 4

#define OBJECT_DOOR 1
#define OBJECT_ECTOPLASMIC_CONVERTER 25
#define OBJECT_ORC_MACHINE 32
#define OBJECT_DEAD_EIRIK 40
#define OBJECT_BROADSWORD 41
#define OBJECT_SCIMITAR 42
#define OBJECT_BOW 45
#define OBJECT_BONE_KEY 46
#define OBJECT_RUNE 49
#define OBJECT_ROD 52
#define OBJECT_BOURBON 53
#define OBJECT_GARLIC_BUD 56
#define OBJECT_WEIRD_DEVICE 57
#define OBJECT_LARGE_BOOK 62
#define OBJECT_MAGIC_TOME 64
#define OBJECT_BLACK_BALL 66
#define OBJECT_POWER_VIAL 69
#define OBJECT_SEEKSHARD 80
#define OBJECT_TORCH 81

#define SPRITE_PLAYER_SMALL 125
#define SPRITE_PLAYER_NORMAL 81
#define SPRITE_PLAYER_NORMAL_REVERSED 147

enum {
  DUNGEON_NONE = 0,
  DUNGEON_MONASTERY,
  DUNGEON_KILKANEY,
  DUNGEON_CAVE
};

#define ROOM_LEVETH_MONOLOGUE 30
#define ROOM_LEVETH_BEDROOM 29

extern char *errmsg[];

extern char *com[];
extern char *verblist[];

typedef struct {
  // char *unused;
  char *allout;
  char *standard;
  char *defensive;
} attack_t;
extern attack_t att[];
extern char *monmsg[];


extern char *obj[];

extern char *mod[];

#endif

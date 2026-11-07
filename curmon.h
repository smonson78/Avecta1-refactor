#ifndef __CURMON_H
#define __CURMON_H

#include <stdint.h>

typedef struct {
  // 0
  uint8_t in_use;
  // 1
  uint8_t damage;
  // 2
  uint8_t max_health;
  // 3
  uint8_t monster_name_id;
  // 4
  uint8_t sprite_id;
  // 5-13
  //         8: will be 80 if you used a seekshard
  //        10: the spell number that was cast
  uint8_t unknown5;
  uint8_t unknown6;
  uint8_t unknown7;
  uint8_t unknown8; // Thing that is being dropped
  uint8_t unknown9;
  // I think this is actually going to be more like a "target of the thing being done"
  uint8_t spell_cast; // This is also sometimes an object number, sometimes a room number
  uint8_t unknown11;
  uint8_t unknown12;
  uint8_t unknown13;
  // 14
  uint8_t spell_units;
  // the command that's just starting and is in progress. This is numbered the same as the 
  // original "iX.c" verb number.
  uint8_t command;
  // 16 - 23
  //        19: the offset into the object of the last thing that was affected by magic
  uint8_t unknown16[8];
  // 24
  uint8_t x;
  // 25
  uint8_t y;
  // 26
  uint8_t old_x; // not 100% sure if these meanings are right
  // 27
  uint8_t old_y;
  // 28-29
  uint8_t unknown28[2];
  // 30
  uint8_t current_room;
  // 31
  uint8_t intelligence_routine;
  // 32
  uint8_t poison_mode_1; // this one is assigned another value from a poison mode
  // 33-34
  uint8_t unknown33[2];
  // 35
  uint8_t stink_lines;
  // 36
  uint8_t unknown36;
  // 37
  uint8_t perception; // Chance out of 100 to find a hidden object
  // 38
  uint8_t controlled;
  // 39
  uint8_t unknown39; // 39 may be "target"
  // 40
  uint8_t unknown40;
  // 41
  uint8_t lit_torches; // actually "turns left on current torch"
  // 42
  uint8_t unlit_torches;
  // 43
  uint8_t base_chance_3_attack;
  // 44
  uint8_t arrows;
  // 45
  uint8_t good_hand;
  // 46
  uint8_t other_hand;
  // 47
  uint8_t poisoned;
  // 48
  uint8_t unknown48;
  // 49
  uint8_t gold_amount;
  // 50
  uint8_t physical_strength;
  // 51
  uint8_t magic_level;
  // 52
  uint8_t seekshard_imprinted_room; 
  // 53
  uint8_t seekshard_imprinted_xy;
  // 54
  uint8_t original_sprite_id;
  // 55-57
  uint8_t unknown55[3];
  // 58
  uint8_t monster_type;
  // 59
  uint8_t unknown59;

} player_t;

typedef struct {
  // 0
  uint8_t in_use;
  // 1
  uint8_t damage;
  // 2
  uint8_t max_health;
  // 3
  uint8_t monster_name_id;
  // 4
  uint8_t sprite_id;
  // 5-9
  uint8_t spells[5];
  // 10
  uint8_t spell_cast;
  // 11
  uint8_t unknown11;
  // 12
  uint8_t unknown12;
  // 13
  uint8_t unknown13;
  // 14
  uint8_t spell_units;
  // the command that's just starting and is in progress. This is numbered the same as the 
  // original "iX.c" verb number.
  uint8_t command;
  // 16 - 23
  //        19: the offset into the object of the last thing that was affected by magic
  uint8_t unknown16[8];
  // 24
  uint8_t x;
  // 25
  uint8_t y;
  // 26
  uint8_t old_x; // not 100% sure if these meanings are right. Might be "destination_x/y"
  // 27
  uint8_t old_y;
  // 28-29
  uint8_t unknown28[2];
  // 30
  uint8_t current_room;
  // 31
  uint8_t intelligence_routine;
  // 32
  uint8_t poison_mode_1; // this one is assigned another value from a poison mode
  // 33-34
  uint8_t unknown33[2];
  // 35
  uint8_t stink_lines;
  // 36
  uint8_t flee;
  // 37
  uint8_t perception; // Chance out of 100 to find a hidden object
  // 38
  uint8_t controlled;
  // 39
  uint8_t unknown39;
  // 40
  uint8_t unknown40;
  // 41
  uint8_t base_chance_1_attack; // These 3 kinda conflict with 46, 37, and 46.
  // 42
  uint8_t base_chance_2_attack;
  // 43
  uint8_t base_chance_3_attack;
  // 44
  uint8_t bane;
  // 45
  uint8_t poison_mode_2; // this one is specifically described as poison mode in comments
  // 46
  uint8_t magic_mode_1;
  // 47
  uint8_t poisoned;
  // 48
  uint8_t unknown48;
  // 49
  uint8_t gold_amount;
  // 50
  uint8_t physical_strength;
  // 51
  uint8_t magic_level;
  // 52
  uint8_t seekshard_imprinted_room;
  // 53
  uint8_t seekshard_imprinted_xy;
  // 54
  uint8_t magic_mode_2;
  // 55-57
  uint8_t unknown55[3];
  // 58
  uint8_t monster_type;
  // 59
  uint8_t unknown59;

} curmon_t;

typedef union {
  player_t player;
  curmon_t monster;
} curmon2_t;

// Don't use this:
extern curmon_t new_curmon[12];

// Use these:
extern player_t *player;
extern player_t *players;
extern curmon_t *monsters;
extern curmon2_t *new_curmon2;


void init_curmon();

#endif
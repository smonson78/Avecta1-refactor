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
  // 5 - the target location for a drag action, a spell cast
  uint8_t action_dest_x;
  // 6
  uint8_t action_dest_y;
  // 7
  uint8_t target;   // Target voice, attack
  // 8
  // During attack_out(), this is what was held in your good hand
  // During cast_in, and others that don't use it, this is set to 0
  // During Examine IN, this is whatever is in either hand
  // Then in Examine OUT, whatever is in here goes into your hand with the message that you "take it out" for examination
  // During Eat, this is the object from your hands that you're about to eat
  // When using an object - in trapres - it's the object you were holding
  uint8_t target_inventory;
  // 9 - an object that is being dragged, examined, or cast upon
  uint8_t target_object;
  // 10: I think this is actually going to be more like a "target of the thing being done"
  uint8_t spell_cast; // This is also sometimes an object number, sometimes a room number
  // 11: time it takes to perform a movement or an attack. 1 is the fastest.
  int8_t speed;
  // 12: armour level, not used
  uint8_t armour;
  // 13: intelligence level, only used in testing for panic
  uint8_t intelligence;
  // 14
  uint8_t spell_units;
  // 15: the command that's just starting and is in progress. This is numbered the same as the 
  // original "iX.c" verb number.
  uint8_t command;
  // 16
  uint8_t time_until_next_move;
  // 17
  uint8_t animation_step;
  uint8_t unknown18;
  // 19: the offset into the object of the last thing that was affected by magic
  uint8_t magic_effect;
  // 20: Original stat before a spell affected the creature. This might speed, strength etc
  uint8_t original_stat;
  // 21
  uint8_t magic_effect_time_remaining;
  // 22 - set by MOVE command
  uint8_t last_position_x;
  // 23 - set by MOVE command
  uint8_t last_position_y;
  // 24
  uint8_t x;
  // 25
  uint8_t y;
  // 26
  uint8_t dest_x;
  // 27
  uint8_t dest_y;
  // 28: destination X for current move
  uint8_t move_to_x;
  // 29: destination Y for current move
  uint8_t move_to_y;
  // 30
  uint8_t current_room;
  // 31
  uint8_t intelligence_routine;
  // 32
  uint8_t poison_mode_1; // this one is assigned another value from a poison mode
  // 33
  uint8_t unknown33;
  // 34
  int8_t unknown34;
  // 35
  uint8_t stink_lines;
  // 36
  uint8_t unknown36;
  // 37
  uint8_t perception; // Chance out of 100 to find a hidden object
  // 38
  uint8_t controlled;
  // 39
  uint8_t unknown39; // 39 is "target" for monsters, but not for players (probably)
  // 40
  uint8_t unknown40;
  // 41
  uint8_t lit_torch_turns; // actually "turns left on current torch"
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
  // 55
  uint8_t unknown55;
  // 56
  uint8_t unknown56;
  // 57
  uint8_t unknown57;
  // 58
  uint8_t monster_type;
  // 59
  uint8_t dungeon;

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
  // 5 - the target location for a drag action, a spell cast
  uint8_t action_dest_x;
  // 6
  uint8_t action_dest_y;
  // 7: target of voice, attack. For sellers - this is used differently: 1=angry, 0=sold you something
  uint8_t target2;
  // 8
  // During attack_out(), this is what was held in your good hand
  // During cast_in, and others that don't use it, this is set to 0
  // During Examine IN, this is whatever is in either hand
  // Then in Examine OUT, whatever is in here goes into your hand with the message that you "take it out" for examination
  // During Eat, this is the object from your hands that you're about to eat
  // When using an object - in trapres - it's the object you were holding
  uint8_t target_inventory;
  // 9 - an object that is being dragged, examined, or cast upon
  uint8_t target_object;
  // 10
  uint8_t spell_cast;
  // 11
  int8_t speed;
  // 12
  uint8_t armour;
  // 13
  uint8_t intelligence;
  // 14
  uint8_t spell_units;
  // 15: the command that's just starting and is in progress. This is numbered the same as the 
  // original "iX.c" verb number.
  uint8_t command;
  // 16
  uint8_t time_until_next_move;
  // 17
  uint8_t animation_step;
  // 18
  uint8_t unknown18;
  // 19
  uint8_t magic_effect;
  // 20
  uint8_t original_stat;
  // 21
  uint8_t magic_effect_time_remaining;
  uint8_t unknown22;
  uint8_t unknown23;
  // 24
  uint8_t x;
  // 25
  uint8_t y;
  // 26
  uint8_t dest_x;
  // 27
  uint8_t dest_y;
  // 28
  uint8_t move_to_x;
  // 29
  uint8_t move_to_y;
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
  // 39: Target of an attack - for monsters. It's [7] for playable characters.
  // Sometimes this is just the monster's own ID. Maybe it's a generic target of whatever they're doing.
  uint8_t attack_target;
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
  // 48: an exit (object ID) that a creature is heading for
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
  // 55
  uint8_t unknown55;
  // 56
  uint8_t unknown56;
  // 57
  uint8_t unknown57;
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
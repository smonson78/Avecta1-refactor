#ifndef __CURMON_H
#define __CURMON_H

#include <stdint.h>

typedef struct {
  // 0
  uint8_t in_use;
  // 1-2
  uint8_t unknown1[2];
  // 3
  uint8_t monster_name_id;
  // 4-13
  uint8_t unknown4[10];
  // 14
  uint8_t spell_units;
  // 15 - 23
  uint8_t unknown15[9];
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
  // 33-36
  uint8_t unknown33[4];
  // 37
  uint8_t unknown37;
  // 38-40
  uint8_t unknown38[3];
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
  // 47-49
  uint8_t unknown47[3];
  // 50
  uint8_t physical_strength;
  // 51
  uint8_t magic_level;
  // 52-53
  uint8_t unknown52[2];
  // 54
  uint8_t magic_mode_2;
  // 55-57
  uint8_t unknown55[3];
  // 58
  uint8_t monster_type;
  // 59
  uint8_t unknown59;

} curmon_t;

extern curmon_t new_curmon[12];

void init_curmon();

#endif
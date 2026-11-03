#include "curmon.h"
#include "globals.h"
#include <libc.h>
#include "tos_compat.h"

// Monster instances
curmon_t new_curmon[12];
player_t *player = (player_t *)&new_curmon[0];
player_t *players = (player_t *)&new_curmon[0];
curmon_t *monsters = &new_curmon[4];

// Maybe a better way when type is shared:
curmon2_t *new_curmon2 = (curmon2_t *)&new_curmon[0];

// If it's curmon[0]:
//   44 - number of arrows owned but also "bane"?
//   49 - gold amount

// If it's curmon[5..11]:
//   31 - intell routine
//   41 - base chance 1 attack
//   42 - base chance 2 attack
//   43 - base chance 3 attack
//   44 - bane
//   45 - poison mode
//   50 - physical strength

// It's almost impossible to move curmon[] from an array of chars to a structure.
// The author has done lots of very strange things such as have pointers that might
// be to different arrays at different times, and the offsets still have to work.
// Goodbye strongly typed language.
// I will leave this one until last.
// or will I?

uint8_t curmon[12][60] = {
  // curmon 0 - the player. Only 15 default bytes populated out of 60.
  {1, 4, 50, 0, 81, 5, 60, 8, 0, 0, 0, 4, 3, 90, 10},
};

// Set up default data for curmon array
void init_curmon() {
  // Set all to zero
  memset(player, 0, sizeof(curmon_t));

  // Initialise the player's data
  player->in_use = 1;
  player->damage = 4;
  player->max_health = 50;
  player->sprite_id = SPRITE_PLAYER_NORMAL;
  player->unknown5 = 5;
  player->unknown6 = 60;
  player->unknown7 = 8;
  //player->unknown1[7] = 0;
  //player->unknown1[8] = 0;
  //player->unknown1[9] = 0;
  player->unknown11 = 4;
  player->unknown12 = 3;
  player->unknown13 = 90;
  player->spell_units = 10;
}

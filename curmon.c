#include "curmon.h"
#include <libc.h>
#include "tos_compat.h"

// Monster instances
curmon_t new_curmon[12];

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

uint8_t curmon[12][60] = {
  // curmon 0 - the player. Only 15 default bytes populated out of 60.
  {1, 4, 50, 0, 81, 5, 60, 8, 0, 0, 0, 4, 3, 90, 10},
};

// Set up default data for curmon array
void init_curmon() {
  // Set all to zero
  memset(&new_curmon, 0, sizeof(curmon_t));

  // Initialise the player's data
  new_curmon[0].in_use = 1;
  new_curmon[0].unknown1[0] = 4;
  new_curmon[0].unknown1[1] = 50;
  //new_curmon[0].unknown1[2] = 0;
  new_curmon[0].unknown1[3] = 81;
  new_curmon[0].unknown1[4] = 5;
  new_curmon[0].unknown1[5] = 60;
  new_curmon[0].unknown1[6] = 8;
  //new_curmon[0].unknown1[7] = 0;
  //new_curmon[0].unknown1[8] = 0;
  //new_curmon[0].unknown1[9] = 0;
  new_curmon[0].unknown1[10] = 4;
  new_curmon[0].unknown1[11] = 3;
  new_curmon[0].unknown1[12] = 90;
  new_curmon[0].unknown1[13] = 10;
}

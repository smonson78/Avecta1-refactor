#ifndef __ROOM_H
#define __ROOM_H

#include <stdint.h>

typedef struct {
  // 3 bytes
  uint8_t room_bg_pattern;
  uint8_t room_bg_style;
  uint8_t room_bg_colour;
} room_background_t;

typedef struct {
  // 3 bytes
  uint8_t object_id;
  uint8_t object_xy;
  uint8_t room_id;
} room_unknown_t;

typedef struct {
  // 9 bytes
  uint8_t unknown[9];
} room_object_t;

typedef struct {

  // 157 bytes
  // - 0   = oldx and oldy (4 bits each) - x in lower, y in upper
  uint8_t room_oldxy;

  // - 1   = another set of xy coords ...apparently byte 1 of a 3-byte structure
  room_unknown_t room_unknown_obj[5];

  // - 16  = background tile 1 pattern
  // - 17  = background tile 1 style
  // - 18  = background tile 1 colour
  // - 19  = background tile 2 pattern
  // - 20  = background tile 2 style
  // - 21  = background tile 2 colour
  room_background_t room_bg[2];

  //actmon(o[22], o[23], o[24], o[25]);

  // - 22  = type of monsters in this room (that can be heard through a door)
  //         (or the type that spawns when you're in there)
  uint8_t room_monster_type;

  // - 23  = unknown flag to actmon
  uint8_t room_monster_nm;

  // - 24  = monsters x
  uint8_t room_monster_x;
  // - 25  = monsters y
  uint8_t room_monster_y;

  // - 26 - percentage chance of monsters appearing I think
  uint8_t room_unknown_2;
  // - 27
  uint8_t room_shopkeeper_id;
  // - 28
  uint8_t room_unknown_4;
  // - 29
  uint8_t room_unknown_5;

  // - 30  = room has background graphics if non-zero
  uint8_t room_has_bg;

  // - 31  = 14 room objects, 9 bytes each
  room_object_t object[13];
  // ... 31  - object 0
  // ... 40  - object 1
  // ... 49  - object 2
  // ... 58  - object 3
  // ... 67  - object 4
  // ... 76  - object 5
  // ... 85  - object 6
  // ... 94  - object 7
  // ... 103 - object 8
  // ... 112 - object 9
  // ... 121 - object 10
  // ... 130 - object 11
  // ... 139 - object 12

  // ... 148 - more unknown data
  uint8_t room_unknown_3[9];

} room_data_t;


typedef struct {
  uint8_t thing_id;
  uint8_t thing_xy;
  uint8_t room_id;

} room_zero_unknown2_t;

typedef struct {
  uint8_t unknown1;
  room_zero_unknown2_t unknown2[52];

} room_zero_data_t;

#endif
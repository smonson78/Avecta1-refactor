#include "globals.h"
#include "cinput.h"
#include "caux.h"
#include "startaux.h"
#include "f5.h"
#include "start.h"

#include "debug.h"

int voice_in(int pc)
{
  check_curmon("start of voice_in()");
  int16_t x, y;
  int ret, tar;
  uint8_t *c = curmon[pc];
  curmon2_t *new_c = &new_curmon2[pc];

  if (outside) {
    return 0;
  }

  if (pc > 0) {
    return 0;
  }

  clrinp();
  domsg(3);
  header(pname);
  winker = pc + 1;
  sgetxy(&x, &y, 0, 0, 0, &ret);
  undomsg();
  winker = 0;
  tar = zline[x][y].monster;

  if (tar == 0 || tar > 3) {
    error(ERROR_NO_CHARACTER);
    return 0;
  }
  
  if (!los(pc,x,y)) {
    error(ERROR_NO_LINE_OF_SIGHT);
    return 0;
  }

  c[7] = tar;
  c[15] = 14;
  c[16] = 1;
  new_c->player.target = tar;
  new_c->player.command = ACTION_VOICE;
  new_c->player.unknown16[0] = 1;

  check_curmon("end of voice_in()");

  return 1;
}

int voice_out(int pc)
{
  check_curmon("start of voice_out()");
  uint8_t *c = curmon[curmon[pc][7]];
  curmon2_t *new_c = &new_curmon2[new_curmon2[pc].player.target];

  curmon[pc][8] = 0;
  curmon[pc][9] = 0;
  new_curmon2[pc].player.unknown8 = 0;
  new_curmon2[pc].player.unknown9 = 0;

  // If already controlled, release them from control
  // if (c[38]) {
  if (new_c->monster.controlled) {
    c[38] = 0;
    new_c->monster.controlled = 0;
    // prnt("-> %s allows %s to make decisions!", pname, name[c[3]], NULL, NULL, NULL, NULL);
    prnt("-> %s allows %s to make decisions!", pname, name[new_c->monster.monster_name_id], 
      NULL, NULL, NULL, NULL);
    return 1;
  }

  // Otherwise, put them under control
  // prnt("-> %s issues orders to %s!", pname, name[c[3]], NULL, NULL, NULL, NULL);
  prnt("-> %s issues orders to %s!", pname, name[new_c->monster.monster_name_id], 
    NULL, NULL, NULL, NULL);

  // if (c[16] != 0) {
  if (new_c->player.unknown16[0] != 0) {
    c[15] = 8;
    c[16] = 1;
    new_c->player.command = ACTION_WAIT;
    new_c->player.unknown16[0] = 1;    
  }

  c[38] = 1;
  new_c->monster.controlled = 1;

  check_curmon("end of voice_out()");

  return 1;
}

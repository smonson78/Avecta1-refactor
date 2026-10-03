#include "globals.h"
#include "gemdefs.h"

int attack(int pc, int xdes, int ydes) {
  int light = rumdata[crum][30],flag = 1,i,bit,xnow,ynow,xadd,yadd;
  xnow = curmon[pc][24];
  ynow = curmon[pc][25];
  bit = curmon[pc][4];
  xadd = xdes - xnow;  /* these better be either -1,0, or 1 */
  yadd = ydes - ynow;
  if (!light && !zline[xnow][ynow][5] && pc > 3) {
    flag = 0;
  }
  Vsync();
  vbl_animation_off();
  if (!flag) {
    storsc(storbuf[pc],16*xnow,16*ynow,0,addr); 
  }
  if (!flag && !zline[xdes][ydes][5]) {
    return 1;
  }
  xnow *= 16;
  ynow *= 16;
  for (i=0;i<8;i += 2) {
    Vsync();
    Vsync();
    storsc(storbuf[pc],xnow + xadd*i,ynow + yadd*i,1,addr);
    storsc(storbuf[pc],xnow + xadd*(2+i),ynow + yadd*(2+i),0,addr);
    blt(bitmap[bit],xnow + xadd*(2+i),ynow + yadd*(2+i),addr);
  }
  for (i=8;i>0;i -= 2) {
    Vsync();
    Vsync();
    storsc(storbuf[pc],xnow + xadd*i,ynow + yadd*i,1,addr);
    storsc(storbuf[pc],xnow + xadd*(i-2),ynow + yadd*(i-2),0,addr);
    if(i != 2 || flag)
      blt(bitmap[bit],xnow + xadd*(i-2),ynow + yadd*(i-2),addr);
  }
  if ( (i = zline[xnow/16][ynow/16][6]) > 0) {
    Vsync();
    blt(bitmap[130 + 2*i],xnow,ynow,addr);
  }
  Vsync();
  vbl_animation_on();
  return 1;
}

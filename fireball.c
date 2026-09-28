
#include "globals.h"
#include "gemdefs.h"
#include "rumdraw.h"

void fireball(int xs, int ys, int xd, int yd, int type)
{
  int i,j,k,l,m,n,q,r,s,t;
  i = 8*(xd - xs);
  j = 8*(yd - ys);
  k = abs(i);
  l = abs(j);
  if(i != 0)
    i /= k;
  if(j != 0)
    j /= l;
  i *= 2;
  j *= 2;
  m = k;
  n = l;
  q = r = s = 0;
  xs *= 16;
  xd *= 16;
  ys *= 16;
  yd *= 16;
  Vsync();
  storsc(firebuff, xs, ys, 0, addr);
  while( (xs/16 != xd/16) || (ys/16 != yd/16) ) {
    if(q == 1) {
      Vsync();
      storsc(firebuff,xs-s,ys-r,1,addr);
      storsc(firebuff,xs,ys,0,addr);
      blt(bitmap[132 + 2*type + rnd(2)],xs,ys,addr);
      q = r = s = 0;
      }
    --m;
    --n;
    if(m < 0) {
      ys += j;
      q = 1;
      m = k;
      r = j;
      }
    if(n < 0) {
      q = 1;
      xs += i;
      n = l;
      s = i;
      }
    }
  Vsync();
  storsc(firebuff,xs-s,ys-r,1,addr);
}

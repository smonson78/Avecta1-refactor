#include "globals.h"
#include "gemdefs.h"

#include "blt.h"
#include "storsc.h"

void explode(int x, int y, int type)
{
   int i,j;
   x *= 16;
   y *= 16;
   Vsync();
   vbl_animation_off();
   storsc(firebuff,x,y,0,addr);
   for(i=1;i<10;i++) {
      Vsync();
      blt(bitmap[132 + 2*type + (i%2)],x,y,addr);
      for(j=0;j<4000;j++);
      Vsync();
      storsc(firebuff,x,y,1,addr);
      }
   vbl_animation_on();
}

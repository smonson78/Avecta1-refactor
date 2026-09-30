#include <stdint.h>

#include "globals.h"

/*******************************************************************************
* STORSC(BUFFER,X,Y,FLAG,ADDR) is a call to store the current screen           *
* at the screen pixel location x,y into the buffer of ints.  The size is the   *
* square size in standard 16 row sprites.  ADDR is the logical screen address. *
* FLAG = 0 means store the screen, FLAG = 1 means restore the screen.          *
*******************************************************************************/


/*
.globl _storsc
.text
_storsc:
  link R14,#-4
  move.l 8(R14),R8
  move.w 12(R14),R0  *X coordinate
  move.w 14(R14),R1  *Y coordinate
  move.w 16(R14),R3  *Flag of function
  move.l 18(R14),R9  *Screen address
  ext.l R1
  ext.l R0
  ext.l R2
  ext.l R3
  clr.l R5
  clr.l R6
  move.w #16,R6
  mulu #160,R1       *Bytes per line of screen
  divu #16,R0        *Mod to find groups of four ints
  move.l R0,R7       *Get a copy of R0
  swap R7            *Low int now holds remainder of division
  mulu #8,R0         *8 bytes per group of 4 ints
  add.l R0,R1        *Add to total bytes
  adda.l R1,R9       *Add to screen address
  clr.l R5
top:
  cmp.w #0,R3
  bne rest:
  move.l (R9),(R8)    *Save screen
  move.l 4(R9),4(R8)
  cmp.w #0,R7         *If character in square then don't restore
  beq proc            *or save adjacent square
  move.l 8(R9),8(R8)
  move.l 12(R9),12(R8)
  bra proc
rest:
  move.l (R8),(R9)    *Restore screen
  move.l 4(R8),4(R9)
  cmp.w #0,R7
  beq proc
  move.l 8(R8),8(R9)
  move.l 12(R8),12(R9)
proc:
  adda.l #16,R8
  adda.l #160,R9
  addq #1,R5
  cmp.w R6,R5
  blt top
  unlk R14
  rts
*/

void storsc(void *buffer, int x, int y, int flag, void *addr)
{
  //   link R14,#-4
  //   move.l 8(R14),R8
  //   move.w 12(R14),R0  *X coordinate
  //   move.w 14(R14),R1  *Y coordinate
  //   move.w 16(R14),R3  *Flag of function
  //   move.l 18(R14),R9  *Screen address
  //   ext.l R1
  //   ext.l R0
  //   ext.l R2 // why is R2 in here???
  //   ext.l R3
  int32_t *r8 = buffer;
  int32_t r0 = x;
  int32_t r1 = y;
  int32_t r3 = flag;
  int32_t *r9 = addr;


  //   clr.l R5
  //   clr.l R6
  int32_t r5 = 0;
  //int32_t r6 = 0;

  //   move.w #16,R6
  int32_t r6 = 16;

  //   mulu #160,R1       *Bytes per line of screen
  r1 *= 160;

  //   divu #16,R0        *Mod to find groups of four ints
  r0 /= 16;
  
  //   move.l R0,R7       *Get a copy of R0
  uint32_t r7; // = x / 16;

  //   swap R7            *Low int now holds remainder of division
  // I now understand how the remainder thing works. 68000 has a weird div which puts the remainder into the upper word.
  r7 = x % 16;

  //   mulu #8,R0         *8 bytes per group of 4 ints
  r0 *= 8;

  //   add.l R0,R1        *Add to total bytes
  r1 += r0;

  //   adda.l R1,R9       *Add to screen address
  r9 += r1 / 4; // divide by 4 due to int width 4 bytes

  //   clr.l R5
  //r5 = 0; // it's already 0.

  // top:
  do {
    //   cmp.w #0,R3
    //   bne rest:
    if (r3 == 0) {
      // Copy 4 bitplanes in two longwords
      //   move.l (R9),(R8)    *Save screen
      r8[0] = r9[0];

      //   move.l 4(R9),4(R8)
      r8[1] = r9[1];

      //   cmp.w #0,R7         *If character in square then don't restore
      //   beq proc            *or save adjacent square
      if (r7 != 0) {
        // Move another 4 bitplanes
        //   move.l 8(R9),8(R8)
        r8[2] = r9[2];
        //   move.l 12(R9),12(R8)
        r8[3] = r9[3];

        //   bra proc
      }
    } else {
      // rest:
      // Do it the opposite way around
      //   move.l (R8),(R9)    *Restore screen
      r9[0] = r8[0];
      //   move.l 4(R8),4(R9)
      r9[1] = r8[1];

      //   cmp.w #0,R7
      //   beq proc
      if (r7 != 0) {
        //   move.l 8(R8),8(R9)
        r9[2] = r8[2];
        //   move.l 12(R8),12(R9)
        r9[3] = r8[3];
      }
    }
    // proc:
    //   adda.l #16,R8
    r8 += 4; // 4 longwords i.e. 16 bytes
    //   adda.l #160,R9
    r9 += 40;

    //   addq #1,R5
    r5++;

    //   cmp.w R6,R5
    //   blt top

    // R6 is 16.
  } while (r5 < r6);

}
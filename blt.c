#include <stdint.h>
#include <libc.h>
/*******************************************************************************
* BLT(SPRITE,XPIX,YPIX,ADDR) takes the address of a sprite in memory = SPRITE, *
* the X,Y coordinates in pixels, and the logical screen address = ADDR and puts*
* your sprite on the screen at that location.                                  *
*******************************************************************************/

// --- 0:  return address
// --- 4:  reserved 4 bytes (maybe this is the return address)
// --- 8:  sprite
// --- 12: xpix
// --- 14: ypix
// --- 16: addr

// Sprites are always 16x16 pixels.

void blt(uint16_t *sprite, int xpix, int ypix, void *addr) {

  // .globl _blt
  // .text
  // _blt:  link R14,#-4          *Allocate stack frame. (a6)

  //        clr.l R0              (d0)
  //        clr.l R1              (d1)
  // We're gonna use d0 as two 16-bit variables in the upper and lower halves
  //uint16_t d0 = 0;
  //uint16_t d0_upper = 0;

  //uint32_t d1 = 0;

  //        move.l 8(R14),R8      *R8 holds the long int address of bitmap in mem (a0)
  //uint16_t *a0 = sprite;

  //        move.w 12(R14),R0     *R0 holds the x-coordinate in screen row pixels
  //d0 = xpix;
  
  //        move.w 14(R14),R1     *R1 holds the y-coordinate in screen column pixels 
  //d1 = ypix;

  //        move.l 16(R14),R9     *Load off screen base address
  uint16_t *dest = addr;

  //        mulu #160,R1         *R1 now holds the number of screen bytes for rows
  //d1 *= 160;
  //uint16_t d1 = ypix * 80;

  //        divu #16,R0          *R0 bot now holds number of two-word groups 
  //d0 /= 16;
  //uint16_t d0 = xpix / 8;

  //        clr.l R2             *Ready R2 for use
  //uint32_t d2 = 0;

  //        move.w R0,R2         *Get bottom word which is # of 2-word groups
  //d2 = d0;

  //        mulu #8,R2           *R2 now holds number of bytes in screen row
  //d2 *= 8;
  //d2 = (xpix / 16) * 8;

  //        swap R0              *R0 now holds the right shift number of sprite
  // ... RIGHT shift????
  // ...which we never use for anything because it'll be overwritten shortly
  //d0_upper = d0;
  // d0 = 0;

  // lemme recheck these maths
  // --> clr.l R0
  // --> move.w 12(R14),R0
  // now R0 = x coord
  // --> divu #16,R0
  // x / 16 so it's the number of 16-bit words now, the start word that contains the sprite
  // --> move.w R0,R2   ...copy it to R2
  // --> mulu #8,R2        now R2 is (x / 16) * 8, comment says "number of bytes in screen row"
  // --> swap R0 ....the weird part. Now R0 will be 0x000E 0000
  
  // --> clr.l R3
  // --> move.w #16,R3
  // --> sub.w R0,R3 ....there's nothing in R0 so what does it do??? R3 = 16 - 0
  // --> move.l R3,R0 .. as a longword, so now R0 is just 0x0000 0010

  //        clr.l R3
  //        move.w #16,R3
  //        sub.w R0,R3
  //uint32_t d3 = 16 - d0;

  //        move.l R3,R0
  //d0_upper = 0;
  //d0 = d3;
  //uint16_t shift = 16 - (xpix / 16); // I don't think I understood the assembler right
  uint16_t shift = 16 - (xpix % 16);

  //  printf("\eE");
  //  printf("\nblit: left shift is %d.\n", shift);

   // for x=238, should be 14 * 8 = 112 or pixel 224 (correct)
  //  printf("starting X byte onscreen is %d", (xpix / 16) * 8);

  //        add.l R2,R1          *R1 now holds starting screen byte address
  // Here, d1 = (ypix * 160) + ((xpix / 16) * 8)   <--- bytes
  //d1 += d2;

  //        add.l R1,R9          *Starting byte for sprite in screen memory in R9 (addr)
  //dest += d1;
  // y * row bytes (320 pixels times 4 bits = 80 words)
  //   plus x / 16 pixels
  //     times 4 bitplanes
  dest += (ypix * 80) + ((xpix / 16) * 4);

  //        clr.l R5              *Zero the row counter 

  // top2:  clr.l R1
  //        clr.l R2
  //        clr.l R3
  //        clr.l R4

  //        clr.l R6
  
  // "row counter"
  uint32_t d5 = 0;

  // top2:
  do {
    uint32_t bitplane0 = 0;
    uint32_t bitplane1 = 0;
    uint32_t bitplane2 = 0;
    uint32_t bitplane3 = 0;

    // This is an accumulator
    uint32_t d6 = 0;

    // Don't you love Atari screen plane maths

    //        move.w (R8),R1        *move bitmap value of plane zero into R2
    bitplane0 = sprite[0];

    //        asl.l R0,R1           *move it the number of bits to the left
    bitplane0 <<= shift;

    //        move.w 2(R8),R2       *move bitmap value of plane one into R3
    bitplane1 = sprite[1]; // (sprite)

    //        asl.l R0,R2
    bitplane1 <<= shift;

    //        move.w 4(R8),R3
    bitplane2 = sprite[2];

    //        asl.l R0,R3
    bitplane2 <<= shift;

    //        move.w 6(R8),R4
    bitplane3 = sprite[3];

    //        asl.l R0,R4
    bitplane3 <<= shift;

    // bitplanes with shift 2 after this should all be 0b 0000 0000 0000 0011 1111 1111 1111 1100

    //        move.l R1,R6          *Put a copy of R3 into R4
    //        or.l R2,R6            *find the plane overlap for nontrivial info
    //        or.l R3,R6
    //        or.l R4,R6
    d6 = bitplane0;
    d6 |= bitplane1;
    d6 |= bitplane2;
    d6 |= bitplane3;
    // OR them all together in the 32-but word

    //        not.l R6            *find the inverse of the nontrivial (1's comp)
    d6 = ~d6; // reverse it 0b 1111 1111 1111 1100 0000 0000 0000 0011

    //        and.w R6,8(R9)        *blanks out screen where the bitmap will appear
    //        and.w R6,10(R9)
    //        and.w R6,12(R9)
    //        and.w R6,14(R9)
    dest[4] &= d6;
    dest[5] &= d6;
    dest[6] &= d6;
    dest[7] &= d6;

    // so punching a hole in the scanline at pixel 224 + 16 = 240
    // ... XXXXX|_______XX  <-- that's the right-hand half of the sprite where it goes over the boundary

    //        swap R6
    // pretty sure we just need the top half now
    d6 = d6 >> 16;

    //        and.w R6,(R9)
    //        and.w R6,2(R9)
    //        and.w R6,4(R9)
    //        and.w R6,6(R9)
    dest[0] &= d6;
    dest[1] &= d6;
    dest[2] &= d6;
    dest[3] &= d6;
    // Now punch out the left-hand half of the sprite
    // ... XXXXX__|XXXXXXX  <---- on the left side of the boundary

    //        or.w R1,8(R9)
    //        or.w R2,10(R9)
    //        or.w R3,12(R9)
    //        or.w R4,14(R9)
    dest[4] |= bitplane0;
    dest[5] |= bitplane1;
    dest[6] |= bitplane2;
    dest[7] |= bitplane3;
    // OR data in here: ________|XXXXXX__ <----(data on the Xs)

    //        swap R1
    //        swap R2
    //        swap R3
    //        swap R4
    // We just want to do the top part that was shifted out now.
    bitplane0 >>= 16;
    bitplane1 >>= 16;
    bitplane2 >>= 16;
    bitplane3 >>= 16;

    //        or.w R1,(R9)
    //        or.w R2,2(R9)
    //        or.w R3,4(R9)
    //        or.w R4,6(R9)
    dest[0] |= bitplane0;
    dest[1] |= bitplane1;
    dest[2] |= bitplane2;
    dest[3] |= bitplane3;
    // ... 

    //        add #8,R8             *done with 8 bytes of memory = 4 16-bit integers
    sprite += 4;

    //        add.l #160,R9         *move to next screen line
    dest += 80;

    //        addq #1,R5            *increment screen line counter
    d5++;
    //        cmp.w #16,R5           *have we done 16 lines yet?
    //        blt top2              *if not then do another line
  } while (d5 < 16);

  // Cconin();
     
}


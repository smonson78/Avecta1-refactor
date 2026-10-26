#include <stdint.h>
#include "globals.h"

#include <libc.h>

#include "tos_compat.h"

/*******************************************************************************
* BLT(SPRITE,XPIX,YPIX,ADDR) takes the address of a sprite in memory = SPRITE, *
* the X,Y coordinates in pixels, and the logical screen address = ADDR and puts*
* your sprite on the screen at that location.                                  *
*******************************************************************************/

// Sprites are always 16x16 pixels.
void blt(sprite_t *sprite, int xpix, int ypix) {

  // debug_printf("> blt() to %d,%d\n", xpix, ypix);
  int16_t *src = sprite->data;

  uint16_t *dest = addr + (ypix * 80) + ((xpix / 16) * 4);
  uint16_t shift = 16 - (xpix % 16);

  for (int i = 0; i < 16; i++) {
    if (ypix + i >= 200) {
      // Don't go off the bottom of video memory
      return;
    }

    // Work register for pixels
    uint32_t d6 = 0;

    uint32_t bitplane0 = src[0] << shift;
    uint32_t bitplane1 = src[1] << shift;
    uint32_t bitplane2 = src[2] << shift;
    uint32_t bitplane3 = src[3] << shift;

    // OR them all together in the 32-bit word
    d6 = bitplane0 | bitplane1 | bitplane2 | bitplane3;

    // Invert it
    d6 = ~d6;

    // Mask out the four destination bitplanes from the lower word
    dest[4] &= d6;
    dest[5] &= d6;
    dest[6] &= d6;
    dest[7] &= d6;

    // Now get the upper word
    d6 = d6 >> 16;

    // Mask out those four bitplanes too
    dest[0] &= d6;
    dest[1] &= d6;
    dest[2] &= d6;
    dest[3] &= d6;

    // Nor OR in the sprite data in the lower words
    dest[4] |= bitplane0;
    dest[5] |= bitplane1;
    dest[6] |= bitplane2;
    dest[7] |= bitplane3;

    // Get the upper words
    bitplane0 >>= 16;
    bitplane1 >>= 16;
    bitplane2 >>= 16;
    bitplane3 >>= 16;

    // OR in the sprite data
    dest[0] |= bitplane0;
    dest[1] |= bitplane1;
    dest[2] |= bitplane2;
    dest[3] |= bitplane3;

    // Move to the next line
    src += 4;

    // And the next screen line
    dest += 80;
  }
}


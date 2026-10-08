#include "st_globals.h"

void filled_rect(int x, int y, int w, int h, int colour) {

  //printf("filled rect %d,%d %dx%d\n", x, y, w, h);
  uint16_t *start_line = globals.video.st_logbase + (y * 80);

  // Draw the left edge of the rectangle first.
  // Find the starting word
  uint16_t *dst = start_line + (x / 16) * 4;
  int offset = x % 16;
 
  int num_blocks = 1 + ((offset + w) / 16);

  // Calculate the left mask
  uint32_t left_mask = 0x0000ffff >> offset;
  // Do we also need to mask the right edge here with 0s?
  if (w < (16 - offset)) {
    left_mask &= 0xffff0000 >> (16 - offset - w);
  }

  // Calculate the right mask - this will only need truncating on the right end
  int right_offset = 15 - ((w + offset) % 16);
  uint32_t right_mask = 0xffff << right_offset;

  uint16_t plane0 = (colour & 1 ? 0xffff : 0);
  uint16_t plane1 = (colour & 2 ? 0xffff : 0);
  uint16_t plane2 = (colour & 4 ? 0xffff : 0);
  uint16_t plane3 = (colour & 8 ? 0xffff : 0);

  uint16_t left_plane0 = plane0 & left_mask;
  uint16_t left_plane1 = plane1 & left_mask;
  uint16_t left_plane2 = plane2 & left_mask;
  uint16_t left_plane3 = plane3 & left_mask;

  uint16_t right_plane0 = plane0 & right_mask;
  uint16_t right_plane1 = plane1 & right_mask;
  uint16_t right_plane2 = plane2 & right_mask;
  uint16_t right_plane3 = plane3 & right_mask;

  // Invert the masks
  left_mask = ~left_mask;
  right_mask = ~right_mask;

  for (int line = 0; line < h; line++) {
    dst = start_line + (line * 80) + (x / 16) * 4;

    for (int block = 0; block < num_blocks; block++) {

      if (block == 0) {
        // First block in line
        // mask out the planes using the inverted mask
        dst[0] &= left_mask;
        dst[1] &= left_mask;
        dst[2] &= left_mask;
        dst[3] &= left_mask;

        // Now insert the colour
        dst[0] |= left_plane0;
        dst[1] |= left_plane1;
        dst[2] |= left_plane2;
        dst[3] |= left_plane3;

      } else if (right_offset && num_blocks > 1 && block == num_blocks - 1) {
        // Last block in line
        // mask out the planes using the inverted mask
        dst[0] &= right_mask;
        dst[1] &= right_mask;
        dst[2] &= right_mask;
        dst[3] &= right_mask;

        // Now insert the colour
        dst[0] |= right_plane0;
        dst[1] |= right_plane1;
        dst[2] |= right_plane2;
        dst[3] |= right_plane3;
      } else {
        // Middle section
        // Insert the colour - no need for masks
        dst[0] = left_plane0;
        dst[1] = left_plane1;
        dst[2] = left_plane2;
        dst[3] = left_plane3;        
      }

      dst += 4;

    }
  }
}

void pattern_rect(int x, int y, int w, int h, uint16_t *bitmap) {

  uint16_t *dest = globals.video.st_logbase + (y * 80) + ((x / 16) * 4);
  uint16_t shift = 16 - (x % 16);

  for (int i = 0; i < 16; i++) {
    // Work register for pixels
    uint32_t d6 = 0;

    uint32_t bitplane0 = bitmap[0] << shift;
    uint32_t bitplane1 = bitmap[16] << shift;
    uint32_t bitplane2 = bitmap[32] << shift;
    uint32_t bitplane3 = bitmap[48] << shift;

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
    bitmap += 1;

    // And the next screen line
    dest += 80;
  }
}


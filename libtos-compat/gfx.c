#include "st_globals.h"

void filled_rect(int x, int y, int w, int h, int colour) {

  printf("filled rect %d,%d %dx%d\n", x, y, w, h);
  uint16_t *start_line = globals.video.st_logbase + (y * 80);

  // Draw the left edge of the rectangle first.
  // Find the starting word
  uint16_t *dst = start_line + (x / 16) * 4;
  int offset = x % 16;

  // // Only do the left edge separately if it's not a full 16-pixels block
  // if (offset != 0) {
  //   uint32_t mask = 0x0000ffff >> offset;

  //   // Do we also need to mask the right edge here with 0s?
  //   if (w < (16 - offset)) {
  //     mask &= 0xffff0000 >> (16 - offset - w);
  //   }

  //   uint16_t plane0 = (colour & 1 ? 0xffff : 0) & mask;
  //   uint16_t plane1 = (colour & 2 ? 0xffff : 0) & mask;
  //   uint16_t plane2 = (colour & 4 ? 0xffff : 0) & mask;
  //   uint16_t plane3 = (colour & 8 ? 0xffff : 0) & mask;

  //   // Invert the mask
  //   mask = ~mask;

  //   // Now go down the left side
  //   for (int i = 0; i < h; i++) {
  //     // mask out the planes using the inverted mask
  //     dst[0] &= mask;
  //     dst[1] &= mask;
  //     dst[2] &= mask;
  //     dst[3] &= mask;

  //     // Now insert the colour
  //     dst[0] |= plane0;
  //     dst[1] |= plane1;
  //     dst[2] |= plane2;
  //     dst[3] |= plane3;

  //     // Move down to next line
  //     dst += 80;
  //   }
  // }

  
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
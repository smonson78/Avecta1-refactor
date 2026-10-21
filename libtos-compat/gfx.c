#include <stdlib.h>
#include <stdint.h>
#include "st_globals.h"

#include "linea.h"


void filled_rect(int x, int y, int w, int h, int colour) {

  if ((x + w) > 320 || (y + h) > 200 || x < 0 || y < 0) {
    printf("Invalid filled rect call %d,%d %dx%d\n", x, y, w, h);
    exit(1);
  }

  // printf("filled rect %d,%d %dx%d colour %d\n", x, y, w, h, colour);
  uint16_t *start_line = globals.video.st_logbase + (y * 80);

  // Draw the left edge of the rectangle first.
  // Find the starting word
  uint16_t *dst = start_line + (x / 16) * 4;
  int offset = x % 16;
 
  int num_blocks = ((offset + w) + 15) / 16;

  // Calculate the left mask
  uint32_t left_mask = 0x0000ffff >> offset;

  // Mask the right edge here also with 0s if needed
  if (w < (16 - offset)) {
    left_mask &= 0xffff << (16 - offset - w);
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
        dst[0] = plane0;
        dst[1] = plane1;
        dst[2] = plane2;
        dst[3] = plane3;        
      }

      dst += 4;

    }
  }
}

// It's hard to tell if this is working or not
// Copy screen rectangle from x,y wxh to dx,dy
void copy_rect(int x, int y, int w, int h, int dx, int dy) {
  // Copying buffer - the size of the screen
  uint16_t buffer[16000];
  uint16_t *p = buffer;

  // printf("Copy rect! %dx%d %dx%d --> %d,%d\n", x, y, w, h, dx, dy);

  // First, let's find the source offset within the screen memory word
  int src_off = x % 16;

  // And the index of that word within the video line. We will use this offset once per line.
  int src_start_index = (x / 16) * 4;

  uint16_t *line = globals.video.st_logbase + (y * 80);

  int num_words = (w + 15) / 16;

  for (int yloop = 0; yloop < h; yloop++) {

    // Start on the first word of the line
    int i = src_start_index;
    uint32_t bitplane0 = line[i++];
    uint32_t bitplane1 = line[i++];
    uint32_t bitplane2 = line[i++];
    uint32_t bitplane3 = line[i++];

    for (int word = 0; word < num_words; word++) {
      // Get the next word of the source data in this accumulator.
      // Look 4 words ahead to get the same colour plane.
      // This read either doesn't help at all, in the case of offset = 0
      // or else it gets the second part of a word that is split across the word boundary.

      if (src_off > 0) {
        // This check should stop it from going off the end of screen memory
        bitplane0 <<= 16;
        bitplane0 |= line[i++];

        bitplane1 <<= 16;
        bitplane1 |= line[i++];

        bitplane2 <<= 16;
        bitplane2 |= line[i++];

        bitplane3 <<= 16;
        bitplane3 |= line[i++];
      } else {
        i += 4;
      }

      // Store 4 bitplane words in the buffer, word-aligned
      *(p++) = (bitplane0 << src_off) >> 16;
      // FIXME just go right
      *(p++) = (bitplane1 << src_off) >> 16;
      *(p++) = (bitplane2 << src_off) >> 16;
      *(p++) = (bitplane3 << src_off) >> 16;
    }

    // Move to next line
    line += 80;
  }

  // Now store in destination
  int dst_start_index = (dx / 16) * 4;
  int dst_off = dx % 16;

  line = globals.video.st_logbase + (dy * 80);

  // Restart at beginning of buffer
  p = buffer;

  for (int yloop = 0; yloop < h; yloop++) {

    int i = dst_start_index;

    // How many pixels are left in each line will be in temp_w
    int temp_w = w;

    // The number of pixels that appear in the final screen word written, or 0 if the last word
    // lines up perfectly with the end of the screen word
    int m = (dst_off + w) % 16;

    for (int word = 0; word < num_words; word++) {
      // Get 4 bitplanes from buffer
      uint16_t bitplane0 = *(p++);
      uint16_t bitplane1 = *(p++);
      uint16_t bitplane2 = *(p++);
      uint16_t bitplane3 = *(p++);

      // Make a mask for the pixels involved for the pixels that fall on the left of the word 
      // boundary
      // e.g.   | 0000111111111111 |
      uint16_t mask = 0xffff >> dst_off;

      // Combine with a mask for the right side if the number of pixels in this word is less
      // than needed to reach the word boundary.
      uint16_t r_mask = 0xffff;
      if (temp_w < (16 - dst_off)) {
        r_mask <<= 16 - m;
        // e.g. | 1111111111000000 |
        mask &= r_mask;
        // e.g. | 0000111111000000 | for a 6-pixel wide image at X=4
      }

      // Less than 16 pixels to go in this word, so we will also need to mask for the length.
      // uint16_t r_mask = 0xffff << (temp_w < 16 ? (16 - temp_w) : 0);
      // mask &= r_mask;

      // Mask out the destination pixels, then copy the bitplane data into position
      line[i] &= ~mask; // these 4 masks appear to be working OK
      line[i] |= (bitplane0 >> dst_off) & mask;

      line[i + 1] &= ~mask;
      line[i + 1] |= (bitplane1 >> dst_off) & mask;

      line[i + 2] &= ~mask;
      line[i + 2] |= (bitplane2 >> dst_off) & mask;

      line[i + 3] &= ~mask;
      line[i + 3] |= (bitplane3 >> dst_off) & mask;

      // Make a mask for the pixels involved for the pixels that fall on the right of the word 
      // boundary, if it hangs over.
      if (dst_off > 0) {
        // This is a right-side mask 
        // If there are less than 16 pixels left to go then we're on the final word, so we'll also
        // mask for the image width here.
        if (temp_w < 16) {
          // Mask off up to the end of the image width
          mask = 0xffff << (16 - m);
        } else {
          // Mask off up to the full size of the source image data word
          mask = 0xffff << (16 - dst_off);
        }
        // Mask out the destination pixels
        line[i + 4] &= ~mask; 
        line[i + 4] |= (bitplane0 << (16 - dst_off)) & mask;
        line[i + 5] &= ~mask;
        line[i + 5] |= (bitplane1 << (16 - dst_off)) & mask;
        line[i + 6] &= ~mask;
        line[i + 6] |= (bitplane2 << (16 - dst_off)) & mask;
        line[i + 7] &= ~mask;
        line[i + 7] |= (bitplane3 << (16 - dst_off)) & mask;
      }

      // Move to next 4 bitplanes
      i += 4;

      // Reduce remaining width counter
      temp_w -= 16;
    }

    // Move to next line
    line += 80;
  }
}

void pattern_rect(int x, int y, int w, int h, uint16_t *bitmap) {

  uint16_t *dest = globals.video.st_logbase + (y * 80) + ((x / 16) * 4);
  uint16_t shift = 16 - (x % 16);

  for (int i = 0; i < 16; i++) {
    // Work register for pixels
    uint32_t d6 = 0;

    // Swap bytes in word
    uint16_t gfx0 = (bitmap[0] << 8) | (bitmap[0] >> 8);
    uint16_t gfx1 = (bitmap[16] << 8) | (bitmap[16] >> 8);
    uint16_t gfx2 = (bitmap[32] << 8) | (bitmap[32] >> 8);
    uint16_t gfx3 = (bitmap[48] << 8) | (bitmap[48] >> 8);

    uint32_t bitplane0 = gfx0 << shift;
    uint32_t bitplane1 = gfx1 << shift;
    uint32_t bitplane2 = gfx2 << shift;
    uint32_t bitplane3 = gfx3 << shift;

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

void blit_surface_to_st_video(int fg_colour, int bg_colour, int x, int y, int width, int height, SDL_Surface *surface) {
  uint16_t *start_line = globals.video.st_logbase + (y * 80);

  // Draw the left edge of the rectangle first.
  // Find the starting word
  uint16_t *dst = start_line + (x / 16) * 4;
  int offset = x % 16;
 
  int num_blocks = 1 + ((offset + width) / 16);

  // Calculate the right mask - this will only need truncating on the right end
  int right_offset = 15 - ((width + offset) % 16);

  for (int line = 0; line < height; line++) {
    uint8_t *src = surface->pixels + (surface->pitch * line);
   
    dst = start_line + (line * 80) + (x / 16) * 4;

    int p = 0;

    for (int block = 0; block < num_blocks; block++) {
      // Get source pixels for this block

      int offset_left = block == 0 ? x % 16 : 0;
      int offset_right = block == num_blocks - 1 ? (15 - (x + width) % 16) : 0;

      // Make mask
      // With left offset
      uint16_t mask = 0xffff >> offset_left;
      // And right offset
      mask &= (0xffff << right_offset);

      uint16_t plane0 = 0;
      uint16_t plane1 = 0;
      uint16_t plane2 = 0;
      uint16_t plane3 = 0;

      for (int i = 0; i < 16 - offset; i++) {
        int colour = src[p++] ? fg_colour : bg_colour;

        plane0 <<= 1;
        plane0 |= colour & 1;

        plane1 <<= 1;
        plane1 |= (colour & 2) >> 1;

        plane2 <<= 1;
        plane2 |= (colour & 4) >> 2;

        plane3 <<= 1;
        plane3 |= (colour & 8) >> 3;

        // Stop at the right edge of the image
        if (p == width) {
          // Skip across
          plane0 <<= offset_right;
          plane1 <<= offset_right;
          plane2 <<= offset_right;
          plane3 <<= offset_right;
          break;
        }
      }

      // Now we have 4 bitplanes for the block lined up against the right side

      if (block == 0) {
        // First block in line
        // mask out the planes using the inverted mask
        dst[0] &= ~mask;
        dst[1] &= ~mask;
        dst[2] &= ~mask;
        dst[3] &= ~mask;

        // Now insert the colour
        dst[0] |= plane0;
        dst[1] |= plane1;
        dst[2] |= plane2;
        dst[3] |= plane3;

      } else if (right_offset && num_blocks > 1 && block == num_blocks - 1) {
        // Last block in line
        // mask out the planes using the inverted mask
        dst[0] &= ~mask;
        dst[1] &= ~mask;
        dst[2] &= ~mask;
        dst[3] &= ~mask;

        // Now insert the colour
        dst[0] |= plane0;
        dst[1] |= plane1;
        dst[2] |= plane2;
        dst[3] |= plane3;
      } else {
        // Middle section
        // Insert the colour - no need for masks
        dst[0] = plane0;
        dst[1] = plane1;
        dst[2] = plane2;
        dst[3] = plane3;
      }

      dst += 4;

    }
  }
}

void render_text_char(int fg_colour, int bg_colour, uint16_t *base, int srcx, int srcy, int dstx, int dsty, 
  int width, int height, int fontwidth, int wrt_mode) {

  // printf("render_text_char at %d,%d size %dx%d:\n", dstx, dsty, width, height);
  // printf("  Cell source coords %d,%d\n", srcx, srcy);

  // Which word contains the first pixel of the character
  int x = dstx / 16;

  // printf("  Destination offset is %d\n", dstx % 16);

  // Screen line on which the character begins
  uint16_t *start_line = globals.video.st_logbase + (dsty * 80);

  // Do once per line in the character cell
  for (int line = 0; line < height; line++) {

    // How many pixels into that word is the first pixel of the character
    int dst_offset = dstx % 16;

    // printf("  Line %d\n", line);

    // Word in that line in which we must start
    uint16_t *dst = start_line + (line * 80) + (x * 4);

    // How many words will we need to do?
    int total_words = (dst_offset + width - 1 + 15) / 16;

    int font_word = (srcx / 16) + (line * fontwidth);
    int font_off = srcx % 16;

    // How many pixels do we actually need for drawing this current word
    int pixels_this_word = dst_offset + width;
    if (pixels_this_word > 16) {
      // Limit to maximum 16
      pixels_this_word = 16;
    }
    // Then remove the offset again
    pixels_this_word -= dst_offset;

    // Get the first font pixels into the accumulator
    uint32_t pixels = base[font_word++] << 16;

    // Number of font pixels in the accumulator
    int font_pixels = 16 - font_off;

    // printf("    Start pixels 0x%08x (%d)", pixels, font_pixels);
    // printf("    Font offs %d\n", font_off);

    int pixels_left = width;

    // Which word are we calculating among the different words that we need to update
    for (int word = 0; word < total_words; word++) {

      // printf("    --- Word number %d\n", word);
      // printf("    Pixels left: %d\n", pixels_left);
      // printf("    This word we need %d font pixels\n", pixels_this_word);
      // printf("    This word we have %d font pixels\n", font_pixels);

      if (font_pixels < pixels_this_word) {
        // Get more font pixels into the low word
        pixels |= base[font_word++];

        // Adjust count. Doesn't matter if this is higher than the actual number.
        font_pixels += 16;
        // printf("    ...getting more pixels.\n");
        // printf("    Start pixels 0x%08x (%d)", pixels, font_pixels);
      }

      // Build the character cell mask for the current word
      // Start with the correct number of bits, lined up on the left edge of the low word
      uint32_t mask = 0xffff0000 >> pixels_this_word;
      // Keep the low word of the mask (that's got the number of pixels we need)
      mask &= 0xffff;
      mask >>= dst_offset; // Now move to the correct position

      // printf("    Mask: 0x%04x\n", mask);
      
      // printf("    Font raw data: %08x\n", pixels);
      // Get a copy and shift the pixels we want to line up with the left edge of the low word
      uint32_t copy_pixels = pixels >> (16 - font_off);
      // printf("    Preshift:      %08x\n", copy_pixels);
      // Shift right to target position
      // TODO ...could do both shifts in one operation

      copy_pixels >>= dst_offset; // now it's lined up with the character cell onscreen

      // Mask it off to the character cell within this screen word
      // printf("    Font data:     %08x\n", copy_pixels);
      copy_pixels &= mask;
      // printf("    Mask:          %04x\n", mask);
      // printf("    Font masked:   %04x\n", copy_pixels);

      //printf("    Screen word:   %04x\n", dst[0]);

      // Mask out the character cell in the current word
      // printf("    Original:      %04x\n", dst[0]);

      if (wrt_mode == 0) {
        // REPLACE mode
        dst[0] &= ~copy_pixels;
        dst[1] &= ~copy_pixels;
        dst[2] &= ~copy_pixels;
        dst[3] &= ~copy_pixels;

        dst[0] |= (fg_colour & 1) ? copy_pixels : 0;
        dst[1] |= (fg_colour & 2) ? copy_pixels : 0;
        dst[2] |= (fg_colour & 4) ? copy_pixels : 0;
        dst[3] |= (fg_colour & 8) ? copy_pixels : 0;
      } else if (wrt_mode == 2) {
        // XOR mode
        dst[0] ^= copy_pixels;
        dst[1] ^= copy_pixels;
        dst[2] ^= copy_pixels;
        dst[3] ^= copy_pixels;
      }
      // Move to next word.
      dst += 4;

      pixels_left -= pixels_this_word;

      // font offset goes up by the number of pixels drawn
      font_off += pixels_this_word;
      // Move font data up to high word if it starts to fall off the end at the right
      if (font_off > 16) {
        font_off -= 16;
        pixels <<= 16;
        font_pixels -= pixels_this_word;
        // printf("    ...moved font data back up to high word: %08x\n", pixels);
        // printf("    ...new font_off is %d\n", font_off);
        // printf("    ...now %d font pixels\n", font_pixels);
      }

      // Set up number of pixels for next word, which will be the number left to draw up to 
      // the maximum of the word size (16).
      pixels_this_word = 16;
      if (pixels_this_word > pixels_left) {
        pixels_this_word = pixels_left;
      }

      // dst_offset only applies to the first word in the line, so zero it out now
      dst_offset = 0;
    }
  }

}

void putpixel(int x, int y, int colour) {
  int dest_word = (y * 80) + ((x / 16) * 4);
  int mask = 0x8000 >> (x % 16);

  // Clear pixel value
  globals.video.st_logbase[dest_word] &= ~mask;
  globals.video.st_logbase[dest_word + 1] &= ~mask;
  globals.video.st_logbase[dest_word + 2] &= ~mask;
  globals.video.st_logbase[dest_word + 3] &= ~mask;

  // Set colour value
  globals.video.st_logbase[dest_word] |= (colour & 1) ? mask : 0;
  globals.video.st_logbase[dest_word + 1] |= (colour & 2) ? mask : 0;
  globals.video.st_logbase[dest_word + 2] |= (colour & 4) ? mask : 0;
  globals.video.st_logbase[dest_word + 3] |= (colour & 8) ? mask : 0;
}

/* Bresenham's Algorithm */
void plotline(int x0, int y0, int x1, int y1, uint32_t colour)
{
	int dx = abs(x1 - x0);
	int dy = -abs(y1 - y0);
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2; /* error value e_xy */

	while(1) {
		putpixel(x0, y0, colour);

    if (x0 == x1 && y0 == y1) {
			break;
    }

		e2 = err << 1;
		if (e2 >= dy) {
			err += dy;
			x0 += sx;
		}

		if (e2 <= dx) {
			err += dx;
			y0 += sy;
		}
	}
}

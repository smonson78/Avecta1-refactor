#include <stdint.h>

#include "globals.h"

/*******************************************************************************
* STORSC(BUFFER,X,Y,FLAG,ADDR) is a call to store the current screen           *
* at the screen pixel location x,y into the buffer of ints.  The size is the   *
* square size in standard 16 row sprites.  ADDR is the logical screen address. *
* FLAG = 0 means store the screen, FLAG = 1 means restore the screen.          *
*******************************************************************************/

void storsc(void *buffer, int x, int y, int flag, void *addr)
{
  int32_t *buf_ptr = buffer;
  int32_t *screen_ptr = addr + (y * 160) + ((x / 16) * 8);

  int alignment = x % 16;

  // top:
  for (int line = 0; line < 16; line++) {
    if (flag == 0) {
      // Copy 4 bitplanes in two longwords
      buf_ptr[0] = screen_ptr[0];
      buf_ptr[1] = screen_ptr[1];

      if (alignment != 0) {
        buf_ptr[2] = screen_ptr[2];
        buf_ptr[3] = screen_ptr[3];
      }
    } else {
      screen_ptr[0] = buf_ptr[0];
      screen_ptr[1] = buf_ptr[1];

      if (alignment != 0) {
        screen_ptr[2] = buf_ptr[2];
        screen_ptr[3] = buf_ptr[3];
      }
    }

    // 4 longwords i.e. 16 bytes
    buf_ptr += 4; 

    // One scanline
    screen_ptr += 40;
  }
}
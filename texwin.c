#include "globals.h"
#include "blt.h"

// Putting some stuff on the screen
void texwin()
{
  int i;

  // Placing bitmap 131 every 16 pixels downward from (240, 128) to (240, 192)
  // and bitmap 199 on the left edge of screen, same vertical positions.
  // Maybe the sides of the scroll.
  for (i = 16; i < 25; i += 2) {
    blt(bitmap[131], 16 * 15, 8 * i, addr);
    blt(bitmap[199], 0, 8 * i, addr);
  }

  // Now the 4 corners. bitmap 129 on the left, 130 on the right.
  blt(bitmap[129], 0, 128, addr);
  blt(bitmap[129], 0, 186, addr);
  blt(bitmap[130], (16 * 15) - 2, 128, addr);
  blt(bitmap[130], (16 * 15) - 2, 186, addr);

  // Reverse video
  v_rvon(handle);

  // Putting some blank text on the screen from column 3 onwards, on lines 17 to 25.
  // That works out to be (24, 136) to (24, 200)
  // printf is an interesting way to acheive a big rectangle.
  for (i = 17; i < 26; i++) {
    vs_curaddress(handle, i, 3);
    printf("                            ");
  }

  // This is all only done once, to set up the screen before the game starts.
  
  // Normal video
  v_rvoff(handle);
}


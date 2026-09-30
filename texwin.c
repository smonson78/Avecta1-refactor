#include "globals.h"
#include "blt.h"

// bitmap 131 would be at offset 131 * 130 bytes = 17030 bytes.
// File shows that there's data in the sprite.
// 0041206 ff 00 ff 00 ff 00 ff 00 ff 80 ff 80 ff 80 ff 80
// 0041226 ff c0 ff c0 ff c0 ff c0 ff 00 ff 00 ff 00 ff 00
// 0041246 fe 00 fe 00 fe 00 fe 00 ff c0 ff c0 ff c0 ff c0
// 0041266 ff 80 ff 80 ff 80 ff 80 ff 40 ff 40 ff 40 ff 40
// 0041306 ff 00 ff 00 ff 00 ff 00 ff 80 ff 80 ff 80 ff 80
// 0041326 ff c0 ff c0 ff c0 ff c0 ff 80 ff 80 ff 80 ff 80
// 0041346 ff 00 ff 00 ff 00 ff 00 fc 80 fc 80 fc 80 fc 80
// 0041366 fe 00 fe 00 fe 00 fe 00 ff 00 ff 00 ff 00 ff 00
// 0041406 00 83 <-- probably rubbish


// Putting the scroll where the text appears on the screen
void texwin()
{
  int i;

  // Placing bitmap 131 every 16 pixels downward from (240, 128) to (240, 192)
  // and bitmap 199 on the left edge of screen, same vertical positions.
  // Maybe the sides of the scroll.
  for (i = 16; i < 25; i += 2) {
    // row 15, i.e. pixel 240
    blt(bitmap[131], 16 * 15, 8 * i, addr);
    blt(bitmap[199], 0, 8 * i, addr);
  }

  // Now the 4 corners. bitmap 129 on the left, 130 on the right.
  blt(bitmap[129], 0, 128, addr);
  blt(bitmap[129], 0, 186, addr);
  
  // left shift: 2 as expected

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
    // 28 chars - or 28 * 8 = 224 pixels. But in the original binary, it goes for another 16 pixels (240). 
    //      0123456789012345678901234567
  }

  // This is all only done once, to set up the screen before the game starts.
  
  // Normal video
  v_rvoff(handle);

  // Test drawing a background sprite:
  // blt(fillpic[1], 0, 0, addr);
  // blt(fillpic[2], 16, 0, addr);
  // blt(fillpic[3], 32, 0, addr);
  // blt(fillpic[4], 48, 0, addr);
  // blt(fillpic[5], 64, 0, addr);
  // Cconin();
  


}


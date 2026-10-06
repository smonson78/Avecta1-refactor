#include "tos_compat.h"

#include "globals.h"
#include "osbind.h"
#include "gemdefs.h"
#include "textbig.h"

void prntbig(char *s, int x, int y)
{
  int i = 0;
  x = 16 * x;
  y = 16 * y;

  while (s[i] != '\0') {
    textbig(s[i], x, y);

    x += 16;
    i++;
    if (x >= 310) {
      x = 0;
      y += 6;
    }
  }
}

int title() {
  int i;
  int16_t rgb[3];

  vs_curaddress(handle, 1, 1);
  v_eeos(handle);
  
  // Set colour 0 (background) to black
  rgb[0] = 0;
  rgb[1] = 0;
  rgb[2] = 0;
  vs_color(handle, 0, rgb);

  Vsync();

  prntbig(" AVECTA I", 4, 1);
  prntbig(" -EBORA-", 4, 5);

  vst_color(handle, 2);

  printf("Copyright Mark Swanson\n");
  printf("Select option: (Q)uit,(N)ew,(R)estore");
  // These calls crash:
  // v_justified(handle, 60, 140, "Copyright Mark Swanson", 16, 0, 0);
  // v_justified(handle, 6, 180, "Select option: (Q)uit,(N)ew,(R)estore", 31, 0, 0);

  // Too much maths
  do {
    do {
      rgb[0] = Random();
      rgb[1] = Random();
      rgb[2] = Random();
      // pointless modding
      rgb[0] %= 1000;
      rgb[1] %= 1000;
      rgb[2] %= 1000;  
      vs_color(handle, 2, rgb);
      i = Bconstat(2);
    } while (i == 0);

    i = Bconin(2) & 0xff;
    
    // Make lowercase
    i = i < 97 ? i + 32 : i;
  } while (i != 'r' && i != 'n' && i != 'q');

  vs_curaddress(handle,1,1);
  v_eeos(handle);

  // Go to game palette
  Setpalette(newpal);

  // ...if it's greater than 'n', it's 'q' or 'r'
  // so 'q' = 0
  //    'n' = 1
  //    'r' = 2
  i = i > 'n' ? 2 * (i - 'q') : 1;

  return i;
}

#include "globals.h"
#include "osbind.h"

int load() {
  long int count = 26000;
  int fhandle, mode = 0;

  // GRAFX.DAT
  fhandle = Fopen(fname, mode);
  if(fhandle < 0)
    return 0;
  Fread(fhandle, count,bitmap[0]);
  Fclose(fhandle);

  // FILL.DAT
  fhandle = Fopen("FILL.DAT",0);
  if (fhandle < 0)
    return 0;
  
  // It's 26000 bytes but we're just going to get the first 40 sprites
  Fread(fhandle, (long)5200, fillpic);
  Fclose(fhandle);

  // OUTSIDE.DAT
  fhandle = Fopen(savname[0], mode);
  if (fhandle < 0)
    return 0;

  // 13400 bytes in total
  Fread(fhandle, count = 12560, rumdata);
  Fread(fhandle, (long)320, putbuf);
  Fread(fhandle, (long)480, trigval);
  Fread(fhandle, (long)40, specbuf);
  Fclose(fhandle);

  return 1;
}


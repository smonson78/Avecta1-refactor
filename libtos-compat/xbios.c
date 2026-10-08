#include <stdio.h>
#include "xbios.h"

#include "globals.h"

int32_t xbios_supexec(int32_t (*func)())
{
  return 0;
}

// Get screen logical base address
void *Logbase() {
  printf("Logbase()\n");
  return globals.video.st_logbase;
}

// Get screen physical base address
void *Physbase() {
  printf("Physbase()\n");
  return globals.video.st_logbase;
}

void Setpalette(int16_t palptr[16]) {
  printf("Setpalette()\n");
}
#include <stdio.h>
#include "xbios.h"

#include "st_globals.h"

int32_t xbios_supexec(int32_t (*func)())
{
  printf("Supexec() running\n");
  return func();
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
	for (int i = 0; i < 16; i++) {
		globals.video.palette[i] = palptr[i] & 0x777;
	}  
}
#include <stdio.h>
#include "globals.h"

#include "linea.h"

// I don't actually know what the default palette is.
int16_t default_palette[16] = {0, 0xfff, 0xf00, 0x0f0, 0x00f, 0xff0, 0xf0f, 0x0ff, 
	0x777, 0xf77, 0x7f7, 0x77f, 0x700, 0x070, 0x007, 0x333 };

void atari_setup() {
  linea_setup();

	globals.video.font = TTF_OpenFont("Bescii-Mono.ttf", 8);
	if (!globals.video.font) {
		fprintf(stderr, "Font not loaded.\n");
		exit(1);
	}

	// For text escape sequences
	globals.video.escape_status = 0;
	
  // keyboard buffer
	globals.keybuf[0] = 0;
	globals.video.st_logbase = malloc(32000);
	// TODO check result
	if (!globals.video.st_logbase) {
		fprintf(stderr, "No video memory.\n");
		exit(1);
	}

	// Clear screen memory
  memset(globals.video.st_logbase, 0, 32000);

	// Initialise palette
	for (int i = 0; i < 16; i++) {
		globals.video.palette[i] = default_palette[i];
	}

	globals.video.current_colour = 1;
	globals.video.current_bgcolour = 0;
}

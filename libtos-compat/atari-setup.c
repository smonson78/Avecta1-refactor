#include <stdio.h>
#include "st_globals.h"

#include "linea.h"
#include "bios.h"

// I don't actually know what the default palette is.
int16_t default_palette[16] = {0, 0x777, 0x700, 0x070, 0x007, 0x770, 0x707, 0x077, 
	0x333, 0x733, 0x373, 0x337, 0x300, 0x030, 0x003, 0x222 };

void atari_setup() {
  linea_setup();

	globals.video.font = TTF_OpenFont("Bescii-Mono.ttf", 8);
	if (!globals.video.font) {
		fprintf(stderr, "Font not loaded.\n");
		exit(1);
	}

	// For text escape sequences
	globals.video.escape_status = 0;
	globals.video.reverse_video = 0;

  // keyboard buffer
	globals.keybuf[0] = 0;

	// Video memory
	globals.video.st_logbase = malloc(32000);
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

	globals.video.current_colour = 15;
	globals.video.current_bgcolour = 0;

	// File handles
	for (int i = 0; i < 64; i++) {
		globals.file_handles[i] = 0;
	}

	// VBL queue
	for (int i = 0; i < 8; i++) {
		VBL_LIST[i] = 0;
	}
	globals.video.vbl_queue = &VBL_LIST;

	// 2D object
	globals.video.udpat_planes = 1;
	globals.video.interior_fill_pattern = 0;
	globals.video.current_2d_colour = 1;
}

void atari_shutdown() {
	free(globals.video.st_logbase);

	for (int i = 0; i < 64; i++) {
		if (globals.file_handles[i] != NULL) {
			fclose(globals.file_handles[i]);
		}
	}	
}
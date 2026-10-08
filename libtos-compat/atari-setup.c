#include <stdio.h>
#include "globals.h"

#include "linea.h"

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
  memset(globals.video.st_logbase, 0xaa, 32000);
}

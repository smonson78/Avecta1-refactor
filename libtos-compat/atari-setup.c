#include <stdio.h>
#include <stdlib.h>
#include "st_globals.h"

#include "linea.h"
#include "bios.h"

// I don't actually know what the default palette is.
int16_t default_palette[16] = {0, 0x777, 0x700, 0x070, 0x007, 0x770, 0x707, 0x077, 
	0x333, 0x733, 0x373, 0x337, 0x300, 0x030, 0x003, 0x222 };

uint16_t predefined_fill_patterns[24][16] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 1
	{0xAAAA, 0, 0xAAAA, 0, 0xAAAA, 0, 0xAAAA, 0, 0xAAAA, 0, 0xAAAA, 0, 0xAAAA, 0, 0xAAAA, 0}, // style 2
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 3
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 4
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 5
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 6
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 7
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 8
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 9
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 10
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 11
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 12
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 13
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 14
	{0, 0, 0x0200, 0, 0x0008, 0, 0x4000, 0, 0, 0, 0x0200, 0, 0x0008, 0, 0x4000, 0}, // style 15
	{
		0xF8F8, 0x6C6C, 0xC6C6, 0x8F8F, 0x1F1F, 0x3636, 0x6363, 0xF1F1,
		0xF8F8, 0x6C6C, 0xC6C6, 0x8F8F, 0x1F1F, 0x3636, 0x6363, 0xF1F1,
	}, // style 16
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 17
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 18
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 19
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 20
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 21
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 22
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 23
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // style 24
};
	
void atari_setup() {
  linea_setup();

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
		globals.file_handles[i] = NULL;
	}
	globals.dta = NULL;

	// VBL queue
	for (int i = 0; i < 8; i++) {
		VBL_LIST[i] = 0;
	}
	globals.video.vbl_queue = &VBL_LIST;

	// 2D object
	globals.video.udpat_planes = 1;
	globals.video.interior_fill_type = 0;
	globals.video.interior_fill_pattern = 0;
	globals.video.current_2d_colour = 1;
}

void atari_shutdown() {
	free(globals.video.st_logbase);

	linea_destroy();

	for (int i = 0; i < 64; i++) {
		if (globals.file_handles[i] != NULL) {
			fclose(globals.file_handles[i]);
		}
	}
}
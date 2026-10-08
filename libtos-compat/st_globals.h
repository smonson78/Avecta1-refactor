#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <pthread.h>
#include <stdio.h>

#include "sdl_graph.h"

#include "xbios.h"

// Global Atari virtual machine state

typedef struct {

	// ST video memory, always 32000 bytes
	uint16_t *st_logbase;

	// Text cursor
	int x_text, y_text;
	int current_colour, current_bgcolour;
	int escape_status;
	int reverse_video;

	// just here to do the built-in font until I get something better
	TTF_Font *font;	

	uint16_t palette[16];

	// VBL handlers installed (pointer to array elsewhere)
	int16_t (*(*vbl_queue)[8])(void);

	// User-defined fill pattern
	int16_t udpat[64];
	int udpat_planes;
	int interior_fill_pattern;
	int current_2d_colour;
} sdl_video_impl_t;

typedef struct {

	// Global lock for machine state
	pthread_mutex_t lock;

	// The current position of the mouse pointer
	int x_mouse, y_mouse;

	// Keyboard buffer
	int keybuf[1];

	// Game will end when set
	int exit;

	sdl_video_impl_t video;

	FILE *file_handles[64];

} globals_t;

#define GLOBAL_LOCK() pthread_mutex_lock(&globals.lock) 
#define GLOBAL_UNLOCK() pthread_mutex_unlock(&globals.lock)

// This is in main.c
extern globals_t globals;

#endif

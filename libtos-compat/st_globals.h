#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <limits.h>
#include <pthread.h>
#include <stdio.h>

#include "sdl_graph.h"

#include "xbios.h"

extern uint16_t predefined_fill_patterns[7][16];

// Data passed TO a file selector, e.g. for save games
typedef struct {
} file_selector_t;

// Data returned FROM a file selector
typedef struct {
	char selected[PATH_MAX];
} file_selector_result_t;

// Global Atari virtual machine state
typedef struct {

	// ST video memory, always 32000 bytes
	uint16_t *st_logbase;

	// Set by game, cleared on vsync flip
	int game_waits_for_vsync;

	// Text cursor
	int x_text, y_text;
	int current_colour, current_bgcolour;
	int escape_status;
	int reverse_video;

	uint16_t palette[16];

	// VBL handlers installed (pointer to array elsewhere)
	int16_t (*(*vbl_queue)[8])(void);

	// VDI:
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
	float x_mouse, y_mouse;
	int mouse_buttons; // bit 0: left, bit 1: right

	file_selector_result_t fs_result;
	int game_waits_for_file_selector;

	// Keyboard buffer
	// TODO do something better than this
	int keybuf[1];

	// Game will end when set
	int exit;

	sdl_video_impl_t video;

	FILE *file_handles[64];
	// Used like stat()
	DTA *dta;

} globals_t;

// #define GLOBAL_LOCK() do {printf("lock\n"); pthread_mutex_lock(&globals.lock);}while(0)
// #define GLOBAL_UNLOCK() do {printf("unlock\n"); pthread_mutex_unlock(&globals.lock);}while(0)
#define GLOBAL_LOCK() pthread_mutex_lock(&globals.lock)
#define GLOBAL_UNLOCK() pthread_mutex_unlock(&globals.lock)

// #define VSYNC_LOCK() pthread_mutex_lock(&globals.video.vsync) 
// #define VSYNC_UNLOCK() pthread_mutex_unlock(&globals.video.vsync)

#define VSYNC_LOCK_FLIP() do{ globals.video.game_waits_for_vsync = 0; } while(0)

// This is in main.c
extern globals_t globals;

#endif

#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <pthread.h>
#include "sdl_graph.h"

// Global Atari virtual machine state

typedef struct {

} sdl_video_impl_t;

typedef struct {

	pthread_mutex_t lock;

	// The current position of the mouse pointer
	int x_mouse, y_mouse;

	// Text cursor
	int x_text, y_text;
	int escape_status;

	// Keyboard buffer
	int keybuf[1];

	// Game will end when set
	int exit;

	// just here to do the built-in font until I get something better
	TTF_Font *font;

} globals_t;

#define GLOBAL_LOCK() pthread_mutex_lock(&globals.lock) 
#define GLOBAL_UNLOCK() pthread_mutex_unlock(&globals.lock)

// This is in main.c
extern globals_t globals;

#endif

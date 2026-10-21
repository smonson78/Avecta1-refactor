#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "sdl_graph.h"

#include "atari-setup.h"
#include "st_globals.h"

// System variables
int screen_width = 320;
int screen_height = 200;

globals_t globals;

#define FRAMELIMIT 50
//#define SHOW_FRAMERATE

// This is the Atari game - Avecta 1 in this case
extern void compat_main();

// Handler for when save dialogue is done
static void SDLCALL OnSaveFileSelected(void* userdata, const char * const * filelist, int filter) {
	GLOBAL_LOCK();
	if (filelist && *filelist) {
		const char* chosen_path = *filelist;
		printf("Selected: %s\n", chosen_path);
		strcpy(globals.fs_result.selected, chosen_path);
	} else {
		globals.fs_result.selected[0] = '\0';
	}

	// Unblock the game thread
	globals.game_waits_for_file_selector = 0;
	GLOBAL_UNLOCK();
}

void *run_atari_game(void *arguments) {
	printf("Game thread running\n");

	compat_main();
	pthread_mutex_lock(&globals.lock);
	globals.exit = true;
	pthread_mutex_unlock(&globals.lock);

	pthread_exit((void*)NULL);
}

int main()
{
	int frame_count = 0;
#if defined SHOW_FRAMERATE
	int next_fps = 0;
#endif

#ifdef FRAMELIMIT
	const int video_frame_ticks = (1000 / FRAMELIMIT);
	int video_tick = 0;
#endif

	uint32_t last_tick, cur_tick;

	printf("Main SDL thread\n");

	// This will run the game itself in a different thread while keeping the virtual Atari 
	// running here.
	pthread_t game_thread;

	setup_sdl(screen_width, screen_height, 2);

	// This variable is used to check when it is time to update game state, or video frames.
	// SDL_GetTicks() gives us the number of milliseconds since the game started. We only
	// intend to call update() every 8 milliseconds so we will divide this by 8.
	last_tick = SDL_GetTicks() / 8;

	globals.exit = false;
	pthread_mutex_init(&globals.lock, NULL);
	atari_setup();
	
	// Spawn a new thread for the game
	if (pthread_create(&game_thread, NULL, run_atari_game, (void*)NULL) != 0) {
			perror("Failed to create thread 1");
			return 1;
	}

	// Game main loop!
	SDL_StartTextInput(video.win);
	while (globals.exit == 0)
	{
		// SDL_GetTicks() gives us the number of milliseconds since the game started.
		cur_tick = SDL_GetTicks();

		// Call update() to update the game's logic every 8 milliseconds (125 times a second)
		while (last_tick < cur_tick / 8) {

			// Update the mouse position
			GLOBAL_LOCK();
			SDL_PumpEvents();
			uint32_t buttons = SDL_GetMouseState(&globals.x_mouse, &globals.y_mouse);
			globals.x_mouse--;
			globals.y_mouse--;
			globals.mouse_buttons = (buttons & SDL_BUTTON_MASK(1) ? 1 : 0) | (buttons & SDL_BUTTON_MASK(2) ? 2 : 0);
			GLOBAL_UNLOCK();

			last_tick++;
		}

#if defined SHOW_FRAMERATE
		// Print out how many frames were drawn over each 1000ms period
		if (next_fps < cur_tick) {
			printf("%d FPS\n", frame_count);
			next_fps += 1000;
			frame_count = 0;
		}
#endif

		// Check for UI events
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			// Handle "Esc" or closing window to quit, no matter what the GameMode is doing.
			// This is just convenient for development really.
			switch (event.type)
			{
				// F1 to quit
			case SDL_EVENT_KEY_DOWN:
				GLOBAL_LOCK();
				if (event.key.key == SDLK_F1) {
					printf("F1 pressed\n");
					globals.exit = true; 
				} else if (event.key.key == SDLK_RETURN) {
					globals.keybuf[0] = '\r';
				}
				GLOBAL_UNLOCK();
				break;

				// Window closed
			case SDL_EVENT_QUIT:
				GLOBAL_LOCK();
				globals.exit = true; 
				GLOBAL_UNLOCK();
				break;

				// Any text entered
			case SDL_EVENT_TEXT_INPUT:
				GLOBAL_LOCK();
				globals.keybuf[0] = event.text.text[0];
				GLOBAL_UNLOCK();
				break;
			}

			// Can't handle in switch due to the custom ID
			if (event.type == custom_event_save_file) {
        SDL_ShowSaveFileDialog(
            OnSaveFileSelected, 
            event.user.data1,   // Pass the thread's data to the callback
            NULL,               // Modal parent window (can be NULL)
            NULL,               // Array of filters
            0,                  // Number of filters
            NULL                // Default starting location
        );
				printf("File selector created!\n");
			}
		}

// Don't worry about this stuff. Most games do limiting to 30 FPS so I've implemented it
// in here if we want to turn it on one day.
#ifdef FRAMELIMIT

		// Frame rate limiting: If it is within 1ms of the next video frame...
		if (cur_tick >= video_tick + video_frame_ticks - 1)
#else
		// Render every frame as soon as possible...
		// (on most computers this will be limited to the monitor's frame rate, which is best)
		if (1)
#endif
		{
			// Let the GameMode object redraw the screen
			// mode->draw();

			// Tell SDL to draw the next video frame
			sdl_flip();

#ifdef FRAMELIMIT
			video_tick = SDL_GetTicks();
#endif

			// Keep track of the number of frames we have drawn for calculating the frames
			// per second.
			frame_count++;
		}
		else
		{
			// Sleep for 1ms (this is only used when limiting the frame rate)
			usleep(1000);
		}
	}

	printf("Exit detected\n");

	// ...The current GameMode has set its "quit" flag, and the main loop has finished.
	// delete mode;

	GLOBAL_LOCK();
	atari_shutdown();
	shutdown_sdl();
	GLOBAL_UNLOCK();

	pthread_mutex_destroy(&globals.lock);

	return EXIT_SUCCESS;
}

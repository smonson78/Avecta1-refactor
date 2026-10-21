#include <stdio.h>
#include <stdlib.h>

#include "sdl_graph.h"
#include "st_globals.h"

#include <SDL3/SDL.h>

#include "bios.h"

video_t video;

int custom_event_save_file;

void sdl_error_fail() {
	fprintf(stderr, "SDL error: %s\n", SDL_GetError());
	exit(1);
}

void setup_sdl(int width, int height, int multiplier)
{
	memset(&video, 0, sizeof(video_t));
	video.width = width;
	video.height = height;
	video.multiplier = multiplier;

	printf("SDL3 init\n");
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		sdl_error_fail();
	}

	int custom_events_start = SDL_RegisterEvents(2);

	custom_event_save_file = custom_events_start;
	//custom_event_load_file = custom_events_start + 1;

#if 1
	/* Print out some video diagnostic info */
	int num_render_drivers = SDL_GetNumRenderDrivers();

	printf("SDL: %d Render drivers on this system.\n", num_render_drivers);

	// for (int i = 0; i < num_render_drivers; i++)
	// {
	// 	SDL_PropertiesID props = SDL_GetRendererProperties(i);
	// 	printf("%d.\t%s\n", i, SDL_Get);
	// 	printf("\tTexture formats:\n");
	// 	unsigned int format;
	// 	for (format = 0; format < video.info.num_texture_formats; format++)
	// 	{
	// 		int temp = video.info.texture_formats[format];
	// 		printf("\t[type=%d order=%d layout=%d bits=%d bytes=%d]\n",
	// 			(temp >> 24) & 0x7,
	// 			(temp >> 20) & 0x7,
	// 			(temp >> 16) & 0x7,
	// 			(temp >> 8) & 0xff,
	// 			temp & 0xff);
	// 	}
	// }
#endif


// #ifdef SDL_FULLSCREEN
// 	/* Fullscreen */
// 	video.win = SDL_CreateWindow("AVECTA 1: Ebora",
// 		width * video.multiplier,
// 		height * video.multiplier,
// 		SDL_WINDOW_FULLSCREEN);
// #else
	/* Windowed */
	video.win = SDL_CreateWindow("AVECTA 1: Ebora",
		width * multiplier,
		height * multiplier,
		0);
// #endif

	if (video.win == NULL) {
		sdl_error_fail();
	}

	printf("SDL: creating renderer for window at %p\n", video.win);

	/* Create the native video renderer */
	video.render_hardware = SDL_CreateRenderer(video.win, NULL);
	printf("SDL: hardware renderer result is %p\n", video.render_hardware);
	if (video.render_hardware == NULL) {
		sdl_error_fail();
	}

	printf("SDL: checking properties\n");

	SDL_PropertiesID props = SDL_GetRendererProperties(video.render_hardware);
	//SDL_GetRendererInfo(video.render_hardware, &video.info);
	printf("SDL renderer in use: %s\n", SDL_GetStringProperty(props, SDL_PROP_RENDERER_NAME_STRING, 0));

	video.render_default = video.render_hardware;

#ifdef SDL_SCREENSURFACE
	//printf("SDL using software rendering.\n");
	/* Create a software renderer */
	video.surf = SDL_CreateSurface(video.width, video.height, SDL_PIXEL_FORMAT);
	if (video.surf == NULL)
	{
		fprintf(stderr, "SDL error: %s\n", SDL_GetError());
		exit(1);
	}
	// SDL_SetSurfaceBlendMode(video.surf, SDL_BLENDMODE_BLEND);
	printf("SDL3 video surface pitch is %d\n", video.surf->pitch);

	video.tex = SDL_CreateTexture(video.render_hardware,  SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING, video.width, video.height);
	SDL_SetTextureBlendMode(video.tex, SDL_BLENDMODE_BLEND);
	SDL_SetTextureScaleMode(video.tex, SDL_SCALEMODE_NEAREST);

	// Make a software renderer for the Surface
	video.render_surface = SDL_CreateSoftwareRenderer(video.surf);
#endif

#ifdef SDL_FULLSCREEN
	// make the scaled rendering look smoother, and crappier.
	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	// Use a lower virtual resolution than the window size
	SDL_SetRenderLogicalPresentation(video.render_hardware, video.width, video.height,
		SDL_LOGICAL_PRESENTATION_LETTERBOX);
#endif

	//-------- TTF font library
	// if (TTF_Init() == -1)
	// {
	// 	fprintf(stderr, "SDL TTF library initialisation failed\n");
	// 	exit(1);
	// }
}

void sdl_flip()
{
	GLOBAL_LOCK();
	// Copy ST video memory to the screen surface
	SDL_LockSurface(video.surf);
	for (int line = 0; line < 200; line++) {
		// Start of ST memory video scanline
		uint16_t *src = globals.video.st_logbase + (line * 80);
		uint32_t *dest = video.surf->pixels + (video.surf->pitch * line);

		for (int word = 0; word < 20; word++) {
			// Read 4 bitplanes
			uint32_t plane0 = src[0];
			uint32_t plane1 = src[1] << 1;
			uint32_t plane2 = src[2] << 2;
			uint32_t plane3 = src[3] << 3;
			src += 4;

			// Copy 16 pixels
			for (int shift = 0; shift < 16; shift++) {
				int colour = (plane0 & 1) | (plane1 & 2) | (plane2 & 4) | (plane3 & 8);

				uint16_t pal = globals.video.palette[colour];
				int red = (pal >> 8) & 0b111;
				red = (red << 5) | (red << 2);
				int green = (pal >> 4) & 0b111;
				green = (green << 5) | (green << 2);
				int blue = pal & 0b111;
				blue = (blue << 5) | (blue << 2);

				// This is NOT causing the segfault, so the surface isn't being corrupted.
				dest[15 - shift] = RGB(red, green, blue);

				// Move down for the next pixel
				plane0 >>= 1;
				plane1 >>= 1;
				plane2 >>= 1;
				plane3 >>= 1;
			}

			dest += 16;
		}
	}


#ifdef SDL_SCREENSURFACE
	float w, h;
	if (SDL_GetTextureSize(video.tex, &w, &h)) {
			// h now contains the texture height in pixels
	}	
	printf("flip: %d, %f, %f\n", video.surf->pitch, w, h);


	/* Update the video memory from the software renderer's surface first */
	SDL_UpdateTexture(video.tex, NULL, video.surf->pixels, video.surf->pitch);

	// Now put the texture on the screen
	SDL_RenderTexture(video.render_hardware, video.tex, NULL, NULL);
#endif
	/* Present the frame onscreen at the next vertical blank */
	SDL_RenderPresent(video.render_hardware);

	SDL_UnlockSurface(video.surf);

	VSYNC_LOCK_FLIP();
	GLOBAL_UNLOCK();	

	// Run the VBL handlers installed on the ST virtual machine
	for (int i = 0; i < 8; i++) {
		if (VBL_LIST[i] != NULL) {
			VBL_LIST[i]();
		}
	}

}

void shutdown_sdl()
{
#ifdef SDL_SCREENSURFACE
	SDL_DestroyRenderer(video.render_surface);
	SDL_DestroySurface(video.surf);
	SDL_DestroyTexture(video.tex);
#endif
	SDL_DestroyRenderer(video.render_hardware);
	SDL_DestroyWindow(video.win);
	SDL_Quit();
}

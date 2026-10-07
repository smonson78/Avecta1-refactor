#include <stdio.h>

#include "sdl_graph.h"

video_t video;

void sdl_error_fail() {
	fprintf(stderr, "SDL error: %s\n", SDL_GetError());
	exit(1);
}

void setup_sdl(int width, int height, int multiplier)
{
	video.width = width;
	video.height = height;
	video.multiplier = multiplier;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		sdl_error_fail();
	}

#if 1
	/* Print out some video diagnostic info */
	int num_render_drivers = SDL_GetNumRenderDrivers();

	printf("SDL: %d Render Drivers on this system.\n", num_render_drivers);

	for (int i = 0; i < num_render_drivers; i++)
	{
		SDL_GetRenderDriverInfo(i, &video.info);
		printf("%d.\t%s\n", i, video.info.name);
		printf("\tTexture formats:\n");
		unsigned int format;
		for (format = 0; format < video.info.num_texture_formats; format++)
		{
			int temp = video.info.texture_formats[format];
			printf("\t[type=%d order=%d layout=%d bits=%d bytes=%d]\n",
				(temp >> 24) & 0x7,
				(temp >> 20) & 0x7,
				(temp >> 16) & 0x7,
				(temp >> 8) & 0xff,
				temp & 0xff);
		}
	}
#endif


#ifdef SDL_FULLSCREEN
	/* Fullscreen */
	video.win = SDL_CreateWindow("SDL2 Test",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
	/* Windowed */
	video.win = SDL_CreateWindow("SDL2 Test",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		video.width * video.multiplier, video.height * video.multiplier, 0);
#endif

	if (video.win == NULL) {
		sdl_error_fail();
	}

	/* Create the native video renderer */
	video.render_hardware = SDL_CreateRenderer(video.win, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (video.render_hardware == NULL) {
		sdl_error_fail();
	}

	SDL_GetRendererInfo(video.render_hardware, &video.info);
	printf("SDL renderer in use: %s\n", video.info.name);

	video.render_default = video.render_hardware;

#ifdef SDL_SCREENSURFACE
	printf("SDL using software rendering.\n");
	/* Create a software renderer */
	video.surf = SDL_CreateRGBSurface(0, video.width, video.height, 32,
			SDL_RMASK, SDL_GMASK, SDL_BMASK, SDL_AMASK);
	if (video.surf == NULL)
	{
		fprintf(stderr, "SDL error: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_SetSurfaceBlendMode(video.surf, SDL_BLENDMODE_BLEND);

	video.tex = SDL_CreateTexture(video.render_hardware,  SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING, video.width, video.height);
	printf("Blend mode: %d\n", SDL_SetTextureBlendMode(video.tex, SDL_BLENDMODE_BLEND));

	// Make a software renderer for the Surface
	video.render_surface = SDL_CreateSoftwareRenderer(video.surf);
#endif

#ifdef SDL_FULLSCREEN
	// make the scaled rendering look smoother, and crappier.
	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	// Use a lower virtual resolution than the window size
	SDL_RenderSetLogicalSize(video.render_hardware, video.width, video.height);
#endif

	//-------- TTF font library
	if (TTF_Init() == -1)
	{
		fprintf(stderr, "SDL TTF library initialisation failed\n");
		exit(1);
	}
}

void sdl_flip()
{
#ifdef SDL_SCREENSURFACE
	/* Update the video memory from the software renderer's surface first */
	SDL_UpdateTexture(video.tex, NULL, video.surf->pixels, video.surf->pitch);
	SDL_RenderCopy(video.render_hardware, video.tex, NULL, NULL);
#endif
	/* Present the frame onscreen at the next vertical blank */
	SDL_RenderPresent(video.render_hardware);
}

void shutdown_sdl()
{
#ifdef SDL_SCREENSURFACE
	SDL_DestroyRenderer(video.render_surface);
	SDL_FreeSurface(video.surf);
	SDL_DestroyTexture(video.tex);
#endif
	SDL_DestroyRenderer(video.render_hardware);
	SDL_DestroyWindow(video.win);
	TTF_Quit();
	SDL_Quit();
}

/* Bresenham's Algorithm */
void plotline(int x0, int y0, int x1, int y1, uint32_t colour)
{
	int dx = abs(x1 - x0);
	int dy = -abs(y1 - y0);
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2; /* error value e_xy */

	while(1)
	{
#ifdef SDL_SCREENSURFACE
		PUTPIXEL_SURF(x0, y0, colour);
#else
		PUTPIXEL_REN(x0, y0, colour);
#endif
		if (x0 == x1 && y0 == y1)
			break;

		e2 = err << 1;
		if (e2 >= dy)
		{
			err += dy;
			x0 += sx;
		}

		if (e2 <= dx)
		{
			err += dx;
			y0 += sy;
		}
	}
}

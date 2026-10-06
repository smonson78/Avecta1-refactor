#ifndef __SDL_GRAPH_H
#define __SDL_GRAPH_H

//#define SDL_FULLSCREEN
//#define SDL_SCREENSURFACE

#include <stdint.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>


#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define SDL_AMASK	0x000000FF
#define SDL_RMASK	0x0000FF00
#define SDL_GMASK	0x00FF0000
#define SDL_BMASK	0xFF000000
#define SDL_GETALPHA(x)	((x & SDL_AMASK))
#define SDL_GETR(x)		((x & SDL_RMASK) >> 8)
#define SDL_GETG(x)		((x & SDL_GMASK) >> 16)
#define SDL_GETB(x)		((x & SDL_BMASK) >> 24)
#define RGB(r,g,b) (SDL_AMASK|(b<<24)|(g<<16)|r<<8)
#else
#define SDL_AMASK	0xFF000000
#define SDL_RMASK	0x00FF0000
#define SDL_GMASK	0x0000FF00
#define SDL_BMASK	0x000000FF
#define SDL_GETALPHA(x)	((x & SDL_AMASK) >> 24)
#define SDL_GETR(x)		((x & SDL_RMASK) >> 16)
#define SDL_GETG(x)		((x & SDL_GMASK) >> 8)
#define SDL_GETB(x)		((x & SDL_BMASK))
#define RGB(r,g,b) (SDL_AMASK|(r<<16)|(g<<8)|b)
#endif

#define PUTPIXEL_REN(x, y, colour) do{SDL_SetRenderDrawColor(video.render_default, \
	SDL_GETR(colour), SDL_GETG(colour), SDL_GETB(colour), SDL_GETALPHA(colour)); \
	SDL_RenderDrawPoint(video.render_default, x, y);}while(0)

#ifdef SDL_SCREENSURFACE
#define PUTPIXEL_SURF(x, y, colour) do {((uint32_t *)((uint8_t *)video.surf->pixels \
		+ (y * video.surf->pitch)))[x] = colour;} while(0)
#endif

typedef struct {
	SDL_Window *win;
	SDL_Renderer *render_hardware;
	SDL_Renderer *render_default;
	SDL_RendererInfo info;

#ifdef SDL_SCREENSURFACE
	// Use a Surface that's blitted to a streaming texture once per frame
	SDL_Surface *surf;
	SDL_Texture *tex;
	SDL_Renderer *render_surface;
#endif

	uint32_t width, height;
	void *pixels;
	int pitch;

} video_t;

extern video_t video;

void setup_sdl(int width, int height);
void shutdown_sdl();
void sdl_flip();

void plotline(int x0, int y0, int x1, int y1, uint32_t colour);

#endif

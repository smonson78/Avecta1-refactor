#include <stdint.h>
#include <stdio.h>

#include <SDL2/SDL_timer.h>

#include "tos.h"

#include "st_globals.h"

// BIOS console input
int32_t Bconin(const int16_t dev) {
    int16_t result;
    do {
        GLOBAL_LOCK();
        result = globals.keybuf[0];

        if (result) {
            globals.keybuf[0] = 0;
            GLOBAL_UNLOCK();
            return result;
        }
        GLOBAL_UNLOCK();
        SDL_Delay(20);

    } while (1);
}

int16_t Bconstat(const int16_t dev) {
    //printf("Bconstat\n");
    int16_t result;
	GLOBAL_LOCK();
    result = globals.keybuf[0];
	GLOBAL_UNLOCK();

    return result != 0 ? -1 : 0;
}

int32_t Cconin()
{
    // 2 = console
    return Bconin(2);
}

int32_t Cconis()
{
    return Bconstat(2);
}

void Cconws(const char* s)
{
}

void Cconout(const uint16_t ch)
{
    printf("Cconout: %c\n", ch);
}

int32_t Cnecin()
{
    int16_t result;
    GLOBAL_LOCK();
    result = globals.keybuf[0];
    globals.keybuf[0] = 0;
    GLOBAL_UNLOCK();
    return result;
}

void *Malloc(int32_t number)
{
    return 0;
}

int32_t Mfree(void *block)
{
    return 0;
}

int32_t Mshrink(void *block, int32_t newsize)
{
    return 0;
}

int32_t Random()
{
    return 0;
}

int16_t Getrez()
{
    return 0;
}

void Vsync()
{
}
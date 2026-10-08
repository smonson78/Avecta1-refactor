#include <stdint.h>
#include <stdio.h>

#include "tos.h"

#include "globals.h"

int16_t (*VBL_LIST[16])(void);

// BIOS console input
int32_t Bconin(const int16_t dev) {
    int16_t result;
	GLOBAL_LOCK();
    result = globals.keybuf[0];
    globals.keybuf[0] = 0;
	GLOBAL_UNLOCK();

    return result;
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
    return 0;
}

int32_t Cconis()
{
    return 0;
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
    return 0;
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
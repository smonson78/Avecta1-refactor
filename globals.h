#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <stdint.h>

int16_t handle;

extern char rumdata[80][157];
extern int16_t oldpal[16];
extern int16_t newpal[16];
extern void *addr;

extern int16_t pxy[8];
extern int16_t bxy[4];
extern int16_t lxy[4];
extern int16_t vxy[4];


#endif
#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <stdint.h>
#include "aes.h"

extern int16_t handle;
extern MFDB psrc, pdes;

extern char rumdata[80][157];
extern int16_t oldpal[16];
extern int16_t newpal[16];
extern void *addr;

extern int16_t pxy[8];
extern int16_t bxy[4];
extern int16_t lxy[4];
extern int16_t vxy[4];

extern char curmon[12][60];
extern int16_t contrl[12], intin[128], intout[128], ptsin[128], ptsout[128];
extern int16_t work_in[11], work_out[57];
extern char specbuf[40], putbuf[320], monbuf[320], junk[20];

extern int crum, objnum, bitmap[200][65];

extern int fromout, new, lev;
extern int monster, hold;
extern int storbuf[12][130], num, time;

extern int row, col;
extern int usedline, prev, prevy;
extern int grflist[13];
extern int winroom;

// Error message strings
extern char *errmsg[];

// Save game path
extern char *path;
extern char *filename;

extern int outside, dungeon;

// If wanted by the police
extern int police;

#endif
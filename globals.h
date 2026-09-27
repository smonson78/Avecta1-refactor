#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <stdint.h>

// This is my Atari LIBC, not a fancy one
#include <libc.h>

#include "aes.h"

// The iX() and oX() mechanism
extern int (*inverb[])();
extern int (*outverb[])();

extern int16_t handle;
extern MFDB psrc, pdes;

// Game data filenames
extern char *savname[];
extern char fname[];

extern int16_t oldpal[16];
extern int16_t newpal[16];
extern void *addr;

extern char pname[20];

extern char weapon[][3];
extern char wepmatx[4][4];
extern char spelinfo[24][3];

extern int16_t pxy[8];
extern int16_t bxy[4];
extern int16_t lxy[4];
extern int16_t vxy[4];

extern uint8_t curmon[12][60];
extern uint8_t permon[][23];
extern char selllist[][4];

// These are all to do with TOS/GEM
extern int16_t contrl[12], intin[128], intout[128], ptsin[128], ptsout[128];
extern int16_t work_in[11], work_out[57];

// buffers probably
extern char monbuf[320], junk[20];
extern int crudbuf[65];
extern uint8_t specbuf[40], putbuf[320];

extern uint8_t zline[16][8][7];
extern uint8_t rumdata[80][157];
extern uint8_t triglist[25];
extern char invtrig[20];

extern uint8_t crumobj[19][9];
extern uint8_t invnpc[4][20];
extern int pursuit[];
extern int mode, dismax;

extern int crum, objnum, bitmap[200][65];

extern int fromout, new, lev;
extern int monster, hold;
extern int storbuf[12][130], num, time;

extern int row, col;
extern int usedline, prev, prevy;
extern int grflist[13];
extern int winroom;

// Save game path
extern char *path;
extern char *filename;

extern int outside, dungeon;

// If wanted by the police
extern int police;

extern int winker, winktime, combat;

extern int firebuff[130];
extern char weight[41];
extern char eats[];
extern uint8_t trigval[80][6];

// Graphics
extern uint16_t fillpic[41][65];

// Magic
extern char spelunit[];

// Words
extern char *posture[];
extern char *wordmod[];
extern char *statword[];
extern char *wepname[];
extern char *spell[];
extern char *name[];
extern char *errmsg[];
extern char *com[];
extern char *verblist[];
extern char *att[][4];
extern char *monmsg[];
extern char *obj[];

extern char *mod[];

#endif
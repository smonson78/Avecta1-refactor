#ifndef __CAUX_H
#define __CAUX_H

int sgetxy(int *x, int *y, int type, int top1, int bot, int *ret);
int los(int pc, int x, int y);
int lom(int pc, int x, int y);
int adjac(int pc, int x, int y);
int rnd(int max);
int move(int obj, int flag, int x, int y);
int top(int line);
int listinv(int pc, char *scratch);
int invent(int pc);
int status(int pc);
int xobj(int thing, int room);
int storobj(int thing, int x, int y);
int remove(int rumobj, int thing);
int takeout(int pc, int thing);
int putaway(int pc, int thing);

#endif
#ifndef __RUMDRAW_H
#define __RUMDRAW_H

int rumdraw(char *pan);
int abs(int x);
void rumclear();
int setfill(int k);
int fillsq(int x, int y);
void click();
void trans(int *bit, int *stor);

#endif
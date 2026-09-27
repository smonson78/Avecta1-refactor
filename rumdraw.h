#ifndef __RUMDRAW_H
#define __RUMDRAW_H

int rumdraw(char *pan);
int abs(int x);
void rumclear();
void setfill(int k);
int fillsq(int x, int y);
void click();
void trans(uint16_t *bit, uint16_t *stor);

#endif
#ifndef __RUMDRAW_H
#define __RUMDRAW_H

void rumdraw(char *pan);
int abs(int x);
void rumclear();
void setfill(int k);
void fillsq(int x, int y);
void click();
void trans(int16_t *bit, int16_t *stor);

#endif
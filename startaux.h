#ifndef __STARTAUX_H
#define __STARTAUX_H

int ruminit();
int init(int flag);
void domsg(int i);
void undomsg();
int console();
void congratulate();
int loadnew();
int error(int i);

// Save palette
int32_t savpal();

#endif
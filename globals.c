#include "globals.h"

// Screen VDI handle
int16_t handle;

int16_t newpal[16] = {
  // Using negative numbers for these hex values is a bit of a wank
  -1912, -120, -1905, -600, -536, -1832, -55, -820, -8, -1793, -311, -1912, -113, -19, -627, 0xfff
};
int16_t oldpal[16];
char rumdata[80][157];

int16_t pxy[8] = {12, 136, 319-75, 199, 12, 134, 319-75, 197};
int16_t bxy[4] = {12, 197, 319-75, 199};
int16_t lxy[4] = {0, 127, 319-75, 128};
int16_t vxy[4] = {255, 0, 255, 199};

// Screen logical address
void *addr;
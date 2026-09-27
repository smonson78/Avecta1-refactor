#include "globals.h"

int i8(int pc)
{
  uint8_t *c = curmon[pc];
  *(c+16) = 4;
  *(c+15) = 8;
  *(c+7) = *(c+8) = *(c+9) = 0;      
  return(1);
}

int o8(int pc)
{
  uint8_t *c = curmon[pc];
  if(pc < 4)
    *(c+18) = 8;
  return(1);
}

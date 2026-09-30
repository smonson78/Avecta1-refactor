#include <stdint.h>
#include "gemdefs.h"

// Switch off the VBL handler
int32_t off() {
  VBL_LIST[2] = NULL;       
  return 0;
}
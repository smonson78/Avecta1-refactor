#include <stdint.h>
#include "tos_compat.h"

#include "gemdefs.h"

#include "tos.h"
#include "bios.h"

// Switch off the VBL handler
int32_t off() {
  VBL_LIST[2] = NULL;       
  return 0;
}
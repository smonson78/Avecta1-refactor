#include <stdint.h>
#include "tos_compat.h"

#include "gemdefs.h"

#include "tos.h"

#if defined TOS_COMPAT
#include "bios.h"
#endif

// Switch off the VBL handler
int32_t off() {
  VBL_LIST[2] = NULL;       
  return 0;
}
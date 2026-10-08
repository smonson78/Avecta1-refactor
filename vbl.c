#include <stdint.h>
#include "tos_compat.h"

#include <libc.h>
#include "bios.h"

#include "animate.h"
#include "gemdefs.h"

/*
.globl _vbl
.globl _animate
.text
_vbl:   link R14,#-4
        move.l #1238,A2
        move.l #_animate,(A2)
        unlk R14
        rts
*/

// Install the vbl routine as a vertical blank handler

int32_t vbl() {
  VBL_LIST[2] = animate;
  return 0;
}
        

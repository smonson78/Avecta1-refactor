#include <stdint.h>
#include "animate.h"
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

#define VBL_LIST ((volatile int16_t (**)())0x4ce)

// Install the vbl routine as a vertical blank handler

void vbl() {
  VBL_LIST[2] = animate;
}
        

#ifndef __GEMDEFS_H
#define __GEMDEFS_H

#include <stdint.h>

#include <gemdos.h>

// Not in my library yet, but needed:
void xbios_37();
void xbios_38_off();
void xbios_38_vbl();

typedef struct fdb {
  void  *fd_addr;     /* Memory address of the target image raster data */
                      /* (Set to NULL to target the physical screen)    */
  int16_t fd_w;       /* Width of the image in pixels                   */
  int16_t fd_h;       /* Height of the image in pixels                  */
  int16_t fd_wdwidth; /* Width of the image in words (pixels / 16)      */
  int16_t fd_stand;   /* Format type: 0 = Device-Specific (Interleaved) */
                      /*              1 = Standard GEM (Standard plane) */
  int16_t fd_nplanes; /* Number of bit planes (e.g., 1, 2, 4, 8)        */
  int16_t fd_r1;      /* Reserved field (always set to 0)               */
  int16_t fd_r2;      /* Reserved field (always set to 0)               */
  int16_t fd_r3;      /* Reserved field (always set to 0)               */
} FDB;


#endif
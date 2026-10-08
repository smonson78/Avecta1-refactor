#ifndef __TOS_COMPAT_H
#define __TOS_COMPAT_H

#include "bios.h"

#if defined TOS_COMPAT
  #define printf(...) compat_printf(__VA_ARGS__)
  #define sprintf compat_sprintf
  #define strlen compat_strlen
  #define strcmp compat_strcmp

  #define memcpy compat_memcpy
  #define memset compat_memset

  #define atoi compat_atoi

  #define exit compat_exit

  // This is not part of TOS, but it IS on a normal POSIX machine and will be called by the NVIDIA driver
  #define remove compat_remove

  #define SHIFTER_PALETTE compat_palette

#else
  #define endianness_fix(buf, words, length) {}
  #define SHIFTER_PALETTE ((uint16_t *)0xFF8240)
#endif

#endif
#ifndef __COMPAT_H
#define __COMPAT_H

#include <stdint.h>

void endianness_fix(void *buf, uint32_t words, int length);

#endif
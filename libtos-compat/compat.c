#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>


// Endianness fix due to different word sizes on different architectures
void endianness_fix(void *buf, uint32_t words, int length) {
  if (length == 16) {

    uint16_t *bitmap = buf;
    for (uint32_t i = 0; i < words; i++) {
      int temp = bitmap[i] << 8;
      bitmap[i] >>= 8;
      bitmap[i] |= temp & 0xff00;
    }
  }
}

int compat_debug_printf(const char *format, ...)
{
  va_list arg;
  int done;

  va_start(arg, format);
  done = vfprintf(stdout, format, arg);
  va_end(arg);

  return done;
}
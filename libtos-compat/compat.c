#include <stdint.h>

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
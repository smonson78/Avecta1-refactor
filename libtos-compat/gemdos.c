#include "gemdos.h"

int32_t gem_super(void *stack)
{
  return 0;
}

int16_t Dgetdrv()
{
  return 0;
}

int16_t Fcreate(const char *fname, int16_t attr)
{
  return 0;
}

int32_t Fopen(const char *fname, int16_t mode)
{
  return 0;
}

void Fclose(const compat_FILE handle)
{
}

int32_t Fwrite(compat_FILE handle, int32_t count, void *buf)
{
  return 0;
}

int32_t Fread(compat_FILE handle, int32_t count, void *buf)
{
  return 0;
}

int32_t Fseek(int32_t offset, int16_t handle, int16_t seekmode)
{
  return 0;
}

void Fsetdta(const DTA *dta)
{
}

int32_t Fsfirst(const char *filename, int16_t attr)
{
  return 0;
}

uint32_t Tgettime()
{
  return 0;
}

uint16_t Tsettime(uint16_t time) {
  return 0;
}

uint16_t Tsetdate(uint16_t date) {
  return 0;
}

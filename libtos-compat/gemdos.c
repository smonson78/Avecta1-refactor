#include <stdio.h>
#include "gemdos.h"

#include "st_globals.h"

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
  int handle;
  printf("Fopen(%s)\n", fname);

  int i;
  for (i = 0; i < 64; i++) {
    if (globals.file_handles[i] == 0) {
      handle = i; 
      break;
    }
  }

  if (i == 64) {
    fprintf(stderr, "Out of file handles.\n");
    exit(1);
  }

  globals.file_handles[i] = fopen(fname, "rb");

  return handle;
}

void Fclose(const compat_FILE handle)
{
  printf("Fclose(%d)\n", handle);
}

int32_t Fwrite(compat_FILE handle, int32_t count, void *buf)
{
  printf("Fwrite %d bytes\n", count);
  return 0;
}

int32_t Fread(compat_FILE handle, int32_t count, void *buf)
{
  printf("Fread %d bytes\n", count);
  if (handle < 0 || handle > 64) {
    fprintf(stderr, "Invalid file handle.\n");
    exit(1);
  }
  FILE *h = globals.file_handles[handle];
  int result = fread(buf, 1, count, h);
  if (result != 1) {
    return 0;
  }

  return count;
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

#include <stdlib.h>
#include <stdio.h>
#include "gemdos.h"

#include "st_globals.h"

int get_free_file_handle() {
  // Find an empty file handle
  for (int i = 0; i < 64; i++) {
    if (globals.file_handles[i] == 0) {
      return i;
    }
  }

  fprintf(stderr, "Out of file handles.\n");
  exit(1);
}

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
  printf("Fcreate(%s)\n", fname);

  GLOBAL_LOCK();
  int index = get_free_file_handle();

  FILE *handle = fopen(fname, "w");
  if (handle) {
    globals.file_handles[index] = handle;
  } else {
    index = -1;
  }
  GLOBAL_UNLOCK();

  // Return the index in the array of the file handle 
  return index;
}

int32_t Fopen(const char *fname, int16_t mode)
{
  printf("Fopen(%s, %d)\n", fname, mode);

  GLOBAL_LOCK();
  int index = get_free_file_handle();

  char posix_mode[3];
  if (mode == 0) {
    strcpy(posix_mode, "r");
  } else if (mode == 1) {
    strcpy(posix_mode, "w");
  } else {
    strcpy(posix_mode, "w+");
  }

  FILE *handle = fopen(fname, "rb");
  if (handle) {
    globals.file_handles[index] = handle;
  } else {
    index = -1;
  }
  GLOBAL_UNLOCK();

  // Return the index in the array of the file handle 
  return index;
}

void Fclose(const compat_FILE index)
{
  printf("Fclose(%d)\n", index);
  GLOBAL_LOCK();
  FILE *handle = globals.file_handles[index];
  if (handle) {
    fclose(handle);
    globals.file_handles[index] = NULL;
  }
  GLOBAL_UNLOCK();
}

int32_t Fwrite(compat_FILE handle, int32_t count, void *buf)
{
  printf("Fwrite %d bytes\n", count);
  GLOBAL_LOCK();
  if (handle < 0 || handle > 64) {
    fprintf(stderr, "Invalid file handle.\n");
    exit(1);
  }
  FILE *h = globals.file_handles[handle];
  int result = fwrite(buf, 1, count, h);
  GLOBAL_UNLOCK();

  if (result != 1) {
    return 0;
  }

  printf("Fwrite wrote %d bytes\n", count);

  return count;
}

int32_t Fread(compat_FILE handle, int32_t count, void *buf)
{
  printf("Fread %d bytes\n", count);
  
  GLOBAL_LOCK();
  if (handle < 0 || handle > 64) {
    fprintf(stderr, "Invalid file handle.\n");
    exit(1);
  }
  FILE *h = globals.file_handles[handle];
  int result = fread(buf, 1, count, h);
  GLOBAL_UNLOCK();

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

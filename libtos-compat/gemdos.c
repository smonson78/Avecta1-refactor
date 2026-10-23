#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

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

  printf("Fcreate returned file handle %d (%p)\n", index, globals.file_handles[index]);
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
    // Reading only
    strcpy(posix_mode, "r");
  } else if (mode == 1) {
    // Writing only
    strcpy(posix_mode, "w");
  } else {
    // Both reading and writing
    strcpy(posix_mode, "w+");
  }

  FILE *handle = fopen(fname, posix_mode);
  if (handle) {
    globals.file_handles[index] = handle;
  } else {
    index = -1;
  }
  printf("Fopen returned file handle %d (%p)\n", index, globals.file_handles[index]);
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

int32_t Fwrite(compat_FILE index, int32_t count, void *buf)
{
  printf("Fwrite %d bytes to file handle %d (%p)\n", count, index, globals.file_handles[index]);
  GLOBAL_LOCK();
  if (index < 0 || index > 64) {
    fprintf(stderr, "Invalid file handle.\n");
    exit(1);
  }

  int result = fwrite(buf, count, 1, globals.file_handles[index]);
  GLOBAL_UNLOCK();

  printf("Fwrite wrote %d bytes\n", result * count);

  if (result != 1) {
    perror("Fwrite");
    return 0;
  }

  return count;
}

int32_t Fread(compat_FILE handle, int32_t count, void *buf)
{
  printf("Fread %d bytes from file handle %d\n", count, handle);
  
  GLOBAL_LOCK();
  if (handle < 0 || handle > 64) {
    fprintf(stderr, "Invalid file handle.\n");
    exit(1);
  }
  FILE *h = globals.file_handles[handle];
  int result = fread(buf, count, 1, h);
  GLOBAL_UNLOCK();

  printf("Fread read %d bytes\n", result * count);

  if (result != 1) {
    perror("Fread");
    return 0;
  }
  
  return count;
}

int32_t Fseek(int32_t offset, int16_t index, int16_t seekmode)
{
  printf("Fseek offset %d, mode %d from file handle %d\n", offset, seekmode, index);
  GLOBAL_LOCK();
  int mode;
  if (seekmode == 0) {
    mode = SEEK_SET;
  } else if (seekmode == 1) {
    mode = SEEK_CUR;
  } else {
    mode = SEEK_END;
  }
  int result = fseek(globals.file_handles[index], offset, mode);
  GLOBAL_UNLOCK();
  return result;
}

void Fsetdta(const DTA *dta)
{
  GLOBAL_LOCK();
  globals.dta = (DTA *)dta;
  GLOBAL_UNLOCK();
}

int32_t Fsfirst(const char *filename, int16_t attr)
{
  GLOBAL_LOCK();
  if (globals.dta) {
    // Get file info
    struct stat statbuf;
    if (stat(filename, &statbuf) == 0) {
      globals.dta->d_length = statbuf.st_size;
    }
  }
  GLOBAL_UNLOCK();

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

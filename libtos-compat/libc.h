#ifndef __LIBC_H
#define __LIBC_H

#include <stdint.h>
#include <stddef.h>

#include "gemdos.h"

#define MALLOC_MIN_ALLOCATION 32

#ifndef __cplusplus
#define NULL ((void *)0)
#endif

extern compat_FILE *compat_stdout;
extern void _exit(uint16_t retval);

struct memblock_t {
    struct memblock_t *next;
    struct memblock_t *prev;
    size_t size;
    uint8_t used;
};

int compat_printf(const char *format, ...);
int compat_sprintf(char *dest, const char *format, ...);
void compat_putchar(char c);
int compat_puts(const char *s);
size_t compat_strlen(const char *s);
void compat_exit(uint16_t retval);
int16_t compat_isdigit(char c);
void compat_memcpy(void *dest, const void *src, size_t bytes);
void compat_strcpy(char *dest, const char *src);
int compat_strcmp(const char *first, const char *second);
int compat_memcmp(const void *s1, const void *s2, size_t n);
void *compat_memset(void *dest, int c, size_t bytes);
int compat_atoi(const char *number);

void compat_abort();

/* For dlmalloc */
#define O_RDWR 0
#define ENOMEM 0
#define EINVAL 0
#define PROT_READ 0
#define PROT_WRITE 0
#define MAP_ANONYMOUS 0
#define MAP_PRIVATE 0

/* Provided by dlmalloc */
void *compat_malloc(size_t size);
void compat_free(void *m);

#endif

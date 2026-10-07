#include "libc.h"
#include "tos.h"

#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

#include "globals.h"

compat_FILE *compat_stdin = (compat_FILE *)0;
compat_FILE *compat_stdout = (compat_FILE *)1;
compat_FILE *compat_stdaux = (compat_FILE *)2;
compat_FILE *compat_stdprn = (compat_FILE *)3;

static const char *digits = "0123456789abcdef";

static int16_t fmt_uint(uint32_t val, int16_t base, char *buf)
{
	char temp[10];
	int16_t size = 0;
	int16_t i;

	/* Handle special case of 0 */
	if (val == 0) {
		buf[0] = '0';
		buf[1] = 0;
		return 1;
	}

	/* Convert number to series of digits */
	while (val)	{
		temp[size++] = digits[val % base];
		val /= base;
	}

	/* Reverse digits into output buffer */
	for (i = 0; i < size; i++) {
		buf[size - i - 1] = temp[i];
	}

	buf[size] = 0;
	return size;
}

static int16_t fmt_int(int32_t val, int16_t base, char *buf)
{
	char temp[10];
	int16_t size = 0;
	int16_t i;
	int16_t neg;

	/* Handle special case of 0 */
	if (val == 0) {
		buf[0] = '0';
		buf[1] = 0;
		return 1;
	}

	if (val < 0) {
		neg = 1;
		val = -val;
		buf[0] = '-';
	} else {
		neg = 0;
	}

	/* Convert number to series of digits */
	while (val)	{
		temp[size++] = digits[val % base];
		val /= base;
	}

	/* Reverse digits into output buffer */
	for (i = 0; i < size; i++) {
		buf[size - i - 1 + neg] = temp[i];
	}

	buf[size + neg] = 0;
	return size + neg;
}

void emit_multi(void (*emit)(char **, char), char **emit_data, const char *s) {
	while (*s) {
		emit(emit_data, *(s++));
	}
}

int compat_vfprintf(void (*emit)(char **, char), char **emit_data, const char *format, va_list arg)
{
	char temp[11];
	uint16_t i;

	while (*format)
	{
		if (*format == '%')
		{
			int16_t width = 0;
			int16_t fill = ' ';
			int16_t ljust = 0;
			uint16_t done = 0;
			uint16_t longarg = 0;
			int16_t length;

			format++;

			/* Get modifiers first */
			while (!done)
			{
				switch (*format)
				{
				case '-':
					format++;
					ljust = 1; break;
				case '0':
					format++;
					fill = '0'; break;
				default:
					done = 1; 	break;
				}
			}

			/* Field width */
			while (isdigit(*format))
			{
				width *= 10;
				width += *format - '0';
				format++;
			}

			if (*format == 'l') {
				longarg = 1;
				format++;
			}

			switch (*format)
			{
			case '%':
				temp[0] = '%';
				length = 1;
				break;

			case 'c':
				temp[0] = (char)__builtin_va_arg(arg, int);
				length = 1;
				break;

			case 'd':
			case 'i':
				if (longarg) {
					length = fmt_int(va_arg(arg, long int), 10, temp);
				} else {
					length = fmt_int(va_arg(arg, int), 10, temp);
				}
				break;

      case 'u':
				if (longarg) {
					length = fmt_uint(va_arg(arg, long int), 10, temp);
				} else {
					length = fmt_uint(va_arg(arg, int), 10, temp);
				}
				break;

			case 'x':
				if (longarg) {
					length = fmt_uint(va_arg(arg, long int), 16, temp);
				} else {
					length = fmt_uint(va_arg(arg, int), 16, temp);
				}
				break;

			case 'p':
				length = fmt_uint(va_arg(arg, long int), 16, temp);
				width = 8;
				fill = '0';
				break;

	    case 's':
	        // FIXME: this has to skip the 'temp' stuff
	        emit_multi(emit, emit_data, va_arg(arg, const char *));
	        length = 0;
              break;

			default:
				/* It's an error! */
				continue;
			}

			if (length != 0) {
				temp[length] = 0;
				if (ljust) {
					emit_multi(emit, emit_data, temp);
				}

				for (i = length; i < width; i++) {
					emit(emit_data, fill);
				}

				if (!ljust) {
					emit_multi(emit, emit_data, temp);
				}
			}
		} else {
			if (*format == '\n') {
				emit(emit_data, '\r');
			}
			emit(emit_data, *format);
		}

		format++;
	}

	return 0;
}

int16_t compat_isdigit(char c)
{
	if (c >= '0' && c <= '9') {
		return 1;
	}
	return 0;
}

void emit_console(char **x, char c) {
	// printf("Emit multi printing letter %c (0x%02x) at position %d,%d\n",
	// 	c, c, globals.x_text, globals.y_text);

	GLOBAL_LOCK();

	if (c == '\e') {
		globals.escape_status = 1;
		GLOBAL_UNLOCK();
		return;
	}

	// Clear screen
	if (globals.escape_status == 1 && c == 'E') {
		globals.x_text = 0;
		globals.y_text = 0;
		globals.escape_status = 0;

		SDL_FillRect(video.surf, NULL, SDL_MapRGB(video.surf->format, 0, 0, 0));

		GLOBAL_UNLOCK();
		return;
	}

	if (c == '\n') {
		globals.x_text = 0;
		if (globals.y_text <= 23) {
			globals.y_text++;
		}

		GLOBAL_UNLOCK();
		return;
	}

	SDL_Colour colour = {255, 255, 255};
	char out[2];
	out[0] = c;
	out[1] = 0;

	// Render into a temporary surface
	SDL_Surface *t = TTF_RenderText_Blended(globals.font, out, colour);
	if (t == NULL) {
		exit(1);
	}

	//SDL_SetSurfaceBlendMode(t, SDL_BLENDMODE_BLEND);
	
	// Define the output rectangle
	SDL_Rect dest;
	dest.x = globals.x_text * 8;
	dest.y = globals.y_text * 8;
	dest.w = 8;
	dest.h = 8;

	// Blit text background
	SDL_FillRect(video.surf, &dest, SDL_MapRGB(video.surf->format, 0, 0, 128));  // Dark blue for test

	// Blit the text
	SDL_BlitSurface(t, NULL, video.surf, &dest);
	SDL_FreeSurface(t);

	// Move along
	if (globals.x_text < 39) {
		globals.x_text++;
	}
	GLOBAL_UNLOCK();
}

void emit_string(char **x, char c) {
	**x = c;
	(*x)++;
}

int compat_printf(const char *format, ...)
{
  va_list arg;
  int done;

  va_start(arg, format);
  done = compat_vfprintf(emit_console, NULL, format, arg);
  va_end(arg);

  return done;
}

int compat_sprintf(char *dest, const char *format, ...)
{
  va_list arg;
  int done;

  va_start(arg, format);
	char *dest_copy = dest;
  done = compat_vfprintf(emit_string, &dest_copy, format, arg);
	*dest_copy = '\0';
  va_end(arg);

  return done;
}

int compat_puts(const char *s)
{
	for (; *s; s++)
	{
		if (*s == '\n')
			Cconout('\r');
		Cconout(*s);
	}
	return 0;
}

void compat_memcpy(void *dest, const void *src, size_t bytes)
{
	while (bytes--) {
		*((char *)dest) = *((char *)src);
		src = (char *)src + 1;
		dest = (char *)dest + 1;
	}
}

size_t compat_strlen(const char *s)
{
	const char *p = s;
	while (*p != '\0') {
		p++;
	}
	return (size_t)p - (size_t)s;
}

void compat_strcpy(char *dest, const char *src)
{
	while (*src != '\0') {
		*(dest++) = *(src++);
	}
}

int compat_strcmp(const char *first, const char *second) {
	while (*first != '\0' && *second != '\0' && *first == *second) {
		first++;
		second++;
	}

	return (*first == *second) ? 0 : 1;
}

void compat_exit(uint16_t retval)
{
	_exit(retval);
}

void compat_abort() {
	printf("abort called\n");
}

void *compat_memset(void *dest, int c, size_t bytes) {
	while (bytes--) {
			*(char *)dest = (char)c;
			dest = (char *)dest + 1;
	}
	return dest;
}

int compat_memcmp(const void *s1, const void *s2, size_t n)
{
	while (n--) {
		if (*((uint8_t *)s1) != *((uint8_t *)s2)) {
				return *(uint8_t *)s1 - *(uint8_t *)s2;
		}
			s1 = (uint8_t *)s1 + 1;
			s2 = (uint8_t *)s2 + 1;
	}
	return 0;
}

int compat_atoi(const char *number) {
	int acc = 0;

	while (isdigit(*number)) {
		acc *= 10;
		acc += *number - '0';
		number++;
	}

	return acc;
}
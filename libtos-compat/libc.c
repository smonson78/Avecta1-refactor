#include "libc.h"
#include "tos.h"

#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

#include "st_globals.h"

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
		globals.video.escape_status = 1;
		GLOBAL_UNLOCK();
		return;
	}

	// Clear screen
	if (globals.video.escape_status == 1 && c == 'E') {
		globals.video.x_text = 0;
		globals.video.y_text = 0;
		globals.video.escape_status = 0;
		
		// TODO because we are actually going to use the ST video memory now
		// SDL_FillRect(video.surf, NULL, SDL_MapRGB(video.surf->format, 0, 0, 0));

		GLOBAL_UNLOCK();
		return;
	}

	if (c == '\n') {
		if (globals.video.y_text <= 23) {
			globals.video.y_text++;
		}

		GLOBAL_UNLOCK();
		return;
	}

	if (c == '\r') {
		globals.video.x_text = 0;

		GLOBAL_UNLOCK();
		return;
	}	

	SDL_Colour colour = {255, 255, 255};
	char out[2];
	out[0] = c;
	out[1] = 0;

	// Render into a temporary 8-bit (palette) surface
	SDL_Surface *t = TTF_RenderText_Solid(globals.video.font, out, colour);
	if (t == NULL) {
		exit(1);
	}

	// The offset (either 0 or 8) within the 16-pixel video word
	int shift = (globals.video.x_text % 2) == 0;
	uint16_t mask = shift ? 0x00ff : 0xff00;

	for (int line = 0; line < 8; line++) {
		// Start of ST memory video scanline

		// The word in which the target character cell exists
		// 640 words = 8 scanlines
		// 4 words = one 16-bit pixel block of 4 scanlines
		uint16_t *dest = globals.video.st_logbase 
			+ (globals.video.y_text * 640) // Start of the line where the character cell starts
			+ (line * 80) // Current line within the character cell
			+ ((globals.video.x_text / 2) * 4); // pixel block (4 words) within that line

		// The source surface is just 8 * 8 pixels, or at least the part we care about
		uint8_t *src = t->pixels + (t->pitch * line);

		// Create empty bitplanes for this line of 8 pixels
		uint16_t plane0 = 0;
		uint16_t plane1 = 0;
		uint16_t plane2 = 0;
		uint16_t plane3 = 0;

		// Move the pixel data into the bitplanes
		for (int pixel = 0; pixel < 8; pixel++) {
			// Just use colour 1 for now
			uint16_t colour = src[pixel] > 0 ? globals.video.current_colour : globals.video.current_bgcolour;

			// Move the bitplanes along 1 pixel before starting
			plane0 <<= 1;
			plane1 <<= 1;
			plane2 <<= 1;
			plane3 <<= 1;

			// Add the new pixel bits to the bitplanes
			plane0 |= colour & 1;
			plane1 |= (colour >> 1) & 1;
			plane2 |= (colour >> 2) & 1;
			plane3 |= (colour >> 3) & 1;
		}

		// Shift for even characters, don't shift for odd
		if (shift) {
			plane0 <<= 8;
			plane1 <<= 8;
			plane2 <<= 8;
			plane3 <<= 8;
		}

		// Mask out the destination pixels in the ST video memory
		dest[0] &= mask;
		dest[1] &= mask;
		dest[2] &= mask;
		dest[3] &= mask;

		// Add the pixel values
		dest[0] |= plane0;
		dest[1] |= plane1;
		dest[2] |= plane2;
		dest[3] |= plane3;
	}

	// SDL_UnlockSurface(video.surf);
	SDL_FreeSurface(t);

	// Move the text cursor along
	if (globals.video.x_text < 39) {
		globals.video.x_text++;
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
	//exit(retval);
	printf("exit()!\n");
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


CC=gcc
SDL_INCLUDE=

DEFS=-DSDL_HWRENDER -DTILEMAP_HWRENDER
#DEFS=-DSDL_HWRENDER -DTILEMAP_HWRENDER -DSDL_FULLSCREEN

LDLIBS=-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_gfx -lm

OPT=-O2
CFLAGS=${OPT} -g -Wall ${SDL_INCLUDE} ${DEFS} -fno-builtin -Wno-builtin-declaration-mismatch -nostdlib -ffreestanding

TOS_OBJS=libc.o aes.o xbios.o tos.o gemdos.o aes_window.o linea.o main.o atari-setup.o compat.o

all: atari-compat.a

atari-compat.a: main.o sdl_graph.o $(TOS_OBJS)
	$(AR) -crs $@ $^

# Header dependencies
sdl_graph.o: sdl_graph.h

clean:
	$(RM) *.o *.elf

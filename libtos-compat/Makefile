CC=gcc
SDL_INCLUDE=
OBJCOPY=objcopy

DEFS=
#DEFS=-DSDL_FULLSCREEN

LDLIBS=-lSDL3 -lm

#OPT=-O2
OPT=-g
DEBUG_FLAGS=-fsanitize=address -fno-omit-frame-pointer
CFLAGS=$(OPT) -Wall $(SDL_INCLUDE) $(DEFS) -fno-builtin -Wno-builtin-declaration-mismatch \
	-nostdlib -ffreestanding $(DEBUG_FLAGS)

TOS_OBJS=libc.o aes.o bios.o xbios.o tos.o gemdos.o aes_window.o linea.o main.o atari-setup.o \
	compat.o gfx.o font0.o font1.o font2.o

all: atari-compat.a

test: main.o sdl_graph.o
	$(CC) $(DEBUG_FLAGS) -o $@ $^ $(LDLIBS)

atari-compat.a: main.o sdl_graph.o $(TOS_OBJS)
	$(AR) -crs $@ $^

# Header dependencies
sdl_graph.o: sdl_graph.h

%.o: %.bin
	$(OBJCOPY) -I binary -O elf64-x86-64 $^ $@ 

clean:
	$(RM) *.o *.elf

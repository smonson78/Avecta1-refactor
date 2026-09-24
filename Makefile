CC=m68k-elf-gcc
CXX=m68k-elf-g++
OBJCOPY=m68k-elf-objcopy

# https://github.com/8l/vlink
VLINK=/home/simon/dev/vlink/vlink
# https://github.com/smonson78/smonson-libtos
LIBTOS=/home/simon/dev/smonson-libtos
# This is where I keep my Hatari virtual hard drive
INSTALL=/home/simon/atari-hd

IDIOTIC_GCC_BULLSHIT=--param=min-pagesize=0
STD_CFLAGS=-m68000 -fomit-frame-pointer -fno-builtin -I. -I$(LIBTOS) -ffreestanding -ffunction-sections $(IDIOTIC_GCC_BULLSHIT)
CFLAGS=-Os -g -Wall $(STD_CFLAGS)
CXXFLAGS=-Os -g -Wall -fno-exceptions -fno-rtti $(STD_CFLAGS)

LDLIBS=-lgcc $(LIBTOS)/libtos.a $(LIBGCC)
LDFLAGS=-m68000 -nostdlib -Wl,--relocatable -L..

TARGET=avecta
EXT=prg
OBJS=start.o globals.o \
	f0.o f1.o f2.o f3.o f4.o f5.o f6.o f7.o f8.o \
	f9.o f10.o f11.o f12.o f13.o f14.o f15.o f16.o
LIBGCC:=$(shell $(CXX) $(CFLAGS) --print-libgcc-file-name)

all: $(TARGET).$(EXT)

$(TARGET).$(EXT): $(TARGET).elf
	@# Strip out .discard section and make .text writable before passing to vlink
	@#$(OBJCOPY) --remove-section=.discard --writable-text $^ ready.o
	$(VLINK) $^ -b ataritos -o $@
	@#$(RM) ready.o

clean:
	$(RM) $(TARGET) *.o *.elf $(TARGET).$(EXT)
	$(RM) *~

$(TARGET).elf: $(OBJS)
	@# Run linker, generate a relocatable object file of the whole project
	$(CXX) -Tatari.ld $(LDFLAGS) $^ $(LDLIBS) -o $(TARGET)1.elf
	$(OBJCOPY) --set-section-flags .data=ALLOC,READONLY,DATA \
		--set-section-flags .text*=ALLOC,CODE \
		$(TARGET)1.elf $@

install: $(TARGET).$(EXT)
	cp --target-directory=$(INSTALL) $^
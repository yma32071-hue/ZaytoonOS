<<<<<<< ours
HOSTCC ?= gcc
HOSTCFLAGS ?= -std=gnu11 -O2 -Wall -Wextra

all: commandline.capp

commandline.builder: commandline.c lib.c lib.h
	$(HOSTCC) $(HOSTCFLAGS) commandline.c lib.c -o $@

commandline.capp: commandline.builder
	./commandline.builder

clean:
	rm -f commandline.builder

.PHONY: all clean
=======
CC = gcc
LD = ld

# ZaytoonOS is currently a 64-bit long-mode kernel, so .capp images are
# linked as flat x86_64 binaries that the kernel can copy to 0x00800000
# and jump into directly. When the kernel grows i386 compatibility mode,
# these flags can switch back to -m32/-melf_i386.
CFLAGS = -m64 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -fcf-protection=none -nostdlib -Wall -Wextra
LDFLAGS = -melf_x86_64 -T linker.ld --oformat binary

ISO_DIR = /workspaces/ZaytoonOS/iso

# Find all .c files EXCEPT lib.c
SRCS = $(filter-out lib.c, $(wildcard *.c))
APPS = $(SRCS:.c=.capp)

.PHONY: all clean install

all: $(APPS)

# Rule compiles the app source ($<) and links it with the helper library
%.capp: %.c lib.c linker.ld
	$(CC) $(CFLAGS) -c $< -o $*.o
	$(CC) $(CFLAGS) -c lib.c -o lib.o
	$(LD) $(LDFLAGS) $*.o lib.o -o $@
	rm $*.o lib.o

install: all
	mkdir -p $(ISO_DIR)
	cp *.capp $(ISO_DIR)/

clean:
	rm -f *.o *.capp a makefile example
>>>>>>> theirs

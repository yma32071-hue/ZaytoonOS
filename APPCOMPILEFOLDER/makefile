CC = gcc
LD = ld

CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib -Wall -Wextra
LDFLAGS = -melf_i386 -Ttext 0x00800000 --oformat binary

ISO_DIR = /workspaces/ZaytoonOS/iso

# Find all .c files EXCEPT lib.c
SRCS = $(filter-out lib.c, $(wildcard *.c))
APPS = $(SRCS:.c=.capp)

.PHONY: all clean install

all: $(APPS)

# Rule compiles the app source ($<) and links it with the helper library
%.capp: %.c lib.c
	$(CC) $(CFLAGS) -c $< -o $*.o
	$(CC) $(CFLAGS) -c lib.c -o lib.o
	$(LD) $(LDFLAGS) $*.o lib.o -o $@
	rm $*.o lib.o

install: all
	mkdir -p $(ISO_DIR)
	cp *.capp $(ISO_DIR)/

clean:
	rm -f *.o *.capp
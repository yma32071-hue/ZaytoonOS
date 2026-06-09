CC = gcc
CFLAGS = -std=gnu11 -O2 -Wall -Wextra

all: commandline.capp

# 1. Compile the host tool executable using standard hosted settings
commandline.builder: commandline.c lib.c lib.h
	$(CC) $(CFLAGS) commandline.c lib.c -o $@

# 2. Run the host tool executable to output the actual commandline.capp file
commandline.capp: commandline.builder
	./commandline.builder

clean:
	rm -f commandline.builder commandline.capp *.o

.PHONY: all clean
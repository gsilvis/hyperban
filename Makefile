CC = gcc
LD = gcc

CFLAGS = -Wall -Wextra -g `pkg-config gtk+-2.0 --cflags` -std=c99
CFLAGS += -Wno-unused-parameter
CFLAGS += -march=native
CFLAGS += -O2
LDFLAGS = $(CFLAGS) `pkg-config gtk+-2.0 --libs`

all: renderer

renderer: renderer.o build.o sokoban.o
	$(LD) -o $@ $^ $(LDFLAGS)

%.o : %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o renderer

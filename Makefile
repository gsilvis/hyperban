CC = gcc
LD = gcc

CFLAGS = -Wall -Wextra -g `pkg-config gtk+-2.0 --cflags`
CFLAGS += -Wno-unused-parameter
LDFLAGS = $(CFLAGS) `pkg-config gtk+-2.0 --libs`

all: renderer

renderer: renderer.o build.o sokoban.o
	$(LD) -o $@ $^ $(LDFLAGS)

%.o : %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o renderer

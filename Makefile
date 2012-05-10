CFLAGS = -Wall -Wextra -std=gnu99
CFLAGS += -Wno-unused-parameter
CFLAGS += -march=native
CFLAGS += -O2
CFLAGS += -g
CFLAGS += `pkg-config gtk+-2.0 --cflags`
LDFLAGS = $(CFLAGS) `pkg-config gtk+-2.0 --libs` -lm

all: renderer

renderer: renderer.o build.o sokoban.o
	gcc -o $@ $^ $(LDFLAGS)

%.o : %.c
	gcc -c -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o renderer

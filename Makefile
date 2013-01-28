CFLAGS = -Wall -Wextra -std=gnu99
CFLAGS += -Wno-unused-parameter -Wno-abi
CFLAGS += -O3 -ffast-math
CFLAGS += -g
CFLAGS += `pkg-config gtk+-2.0 gthread-2.0 --cflags`
LDFLAGS = $(CFLAGS) `pkg-config gtk+-2.0 gthread-2.0 --libs` -lm

CFILES = renderer.c $(wildcard gui/*.c) $(wildcard graph/*.c)
OFILES = $(patsubst %.c, %.o, $(CFILES))

all: renderer

renderer: $(OFILES)
	gcc -o $@ $^ $(LDFLAGS)

%.o : %.c
	gcc -c -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o renderer

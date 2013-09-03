CFLAGS = -Wall -Wextra -std=gnu99
CFLAGS += -Wno-unused-parameter -Wno-abi
CFLAGS += -O2 -ffast-math -march=native
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
	rm -f $(OFILES) renderer

CFLAGS = -Wall -Wextra -std=gnu99
CFLAGS += -Wno-unused-parameter -Wno-abi
CFLAGS += -march=native
CFLAGS += -O3 -ffast-math -flto
CFLAGS += -g
CFLAGS += `pkg-config gtk+-2.0 gthread-2.0 --cflags`
LDFLAGS = $(CFLAGS) `pkg-config gtk+-2.0 gthread-2.0 --libs` -lm

CFILES = renderer.c build.c sokoban.c level.c graph.c audit.c board.c
CFILES += rendering.c cairo_helper.c matrix.c
OFILES = $(patsubst %.c, %.o, $(CFILES))

all: renderer

renderer: $(OFILES)
	gcc -o $@ $^ $(LDFLAGS)

%.o : %.c
	gcc -c -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o renderer

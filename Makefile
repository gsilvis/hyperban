CFLAGS = -Wall -Wextra -std=gnu99
CFLAGS += -Wno-unused-parameter
CFLAGS += -march=native
CFLAGS += -O2
CFLAGS += -g
CFLAGS += `pkg-config gtk+-2.0 --cflags`
LDFLAGS = $(CFLAGS) `pkg-config gtk+-2.0 --libs` -lm

CFILES = renderer.c build.c sokoban.c level.c graph.c
OFILES = $(patsubst %.c, %.o, $(CFILES))

all: renderer

renderer: $(OFILES)
	gcc -o $@ $^ $(LDFLAGS)

%.o : %.c
	gcc -c -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OFILES) renderer

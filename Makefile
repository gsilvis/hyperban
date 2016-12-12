CFLAGS = -Wall -Wextra -std=gnu99
CFLAGS += -Wno-unused-parameter -Wno-abi
CFLAGS += -g
LDFLAGS = $(CFLAGS) -lm

CFILES = renderer.c $(wildcard gui/*.c) $(wildcard graph/*.c)
OFILES = $(patsubst %.c, %.o, $(CFILES))

all: renderer.js

renderer.js: blob-scalar.o
	$(CC) --js-library cairo.js -s EXPORTED_FUNCTIONS="['_renderer_draw']" -o $@ $^ $(LDFLAGS)

blob-scalar.o: blob.o
	opt -scalarizer -sroa < $^ > $@

blob.o: $(OFILES)
	$(CC) -o $@ $^

%.o : %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OFILES) renderer.js blob.o blob-scalar.o

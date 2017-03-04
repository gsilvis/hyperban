CFLAGS = -Wall -Wextra -std=gnu99
CFLAGS += -Wno-unused-parameter -Wno-abi
CFLAGS += -g -O2
LDFLAGS = $(CFLAGS) -lm

EMCC_FLAGS = --js-library cairo.js -s EXPORTED_FUNCTIONS="['_draw_board','_load_board','_do_keypress']"
EMCC_FLAGS += --preload-file levels/easy.txt

CFILES = renderer.c $(wildcard gui/*.c) $(wildcard graph/*.c)
OFILES = $(patsubst %.c, %.o, $(CFILES))

all: renderer.js

renderer.js: $(OFILES) cairo.js
	emcc $(EMCC_FLAGS) -o $@ $(filter-out %.js, $^) $(LDFLAGS)

%.o : %.c
	emcc -c -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OFILES) renderer.js 

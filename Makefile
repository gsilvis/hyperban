CFLAGS = -Wall -Wextra -std=gnu99
CFLAGS += -Wno-unused-parameter -Wno-abi
CFLAGS += -g -O2
LDFLAGS = $(CFLAGS) -lm

EMCC_FLAGS = --js-library cairo.js -s EXPORTED_FUNCTIONS="['_js_draw_graph','_js_get_pos','_js_load_board','_js_dump_board','_js_do_move','_js_undo_move']"
EMCC_FLAGS += --preload-file levels

CFILES = renderer.c $(wildcard gui/*.c) $(wildcard graph/*.c)
OFILES = $(patsubst %.c, %.o, $(CFILES))

all: renderer.js levels.json

renderer.js: $(OFILES) cairo.js
	emcc $(EMCC_FLAGS) -o $@ $(filter-out %.js, $^) $(LDFLAGS)

levels.json: $(wildcard levels/*.txt)
	ls levels/*.txt | jq -Rs 'split("\n") | .[0:-1]' > $@

%.o : %.c
	emcc -c -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OFILES) renderer.js 

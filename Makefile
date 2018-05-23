CFLAGS = -Wall -Wextra -std=gnu99
CFLAGS += -Wno-unused-parameter -Wno-abi -Wno-unused-function -Wno-unused-const-variable
#CFLAGS += -g -O2
CFLAGS += --closure 1 -O3
LDFLAGS = $(CFLAGS) -lm

EXPFUNCS = "['_js_draw_graph','_js_get_pos','_js_load_board','_js_dump_board','_js_do_move','_js_undo_move','_js_edit_board','_js_get_unsolved','_js_get_moves','_js_set_custom_projection']"

EMCC_FLAGS = --js-library module/cairo.js
EMCC_FLAGS += -s EXPORTED_FUNCTIONS=$(EXPFUNCS)
EMCC_FLAGS += --llvm-lto 1
EMCC_FLAGS += --preload-file levels
EMCC_FLAGS += -s MODULARIZE=1
EMCC_FLAGS += -s EXPORT_NAME="'Hyperban'"
EMCC_FLAGS += -s 'EXTRA_EXPORTED_RUNTIME_METHODS=["FS"]'

CFILES = module/renderer.c $(wildcard module/gui/*.c) $(wildcard module/graph/*.c)
OFILES = $(patsubst %.c, %.o, $(CFILES))

all: renderer.js

renderer.js: $(OFILES) module/cairo.js
	emcc $(EMCC_FLAGS) -o $@ $(filter-out %.js, $^) $(LDFLAGS)

%.o : %.c
	emcc -c -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OFILES) renderer.js

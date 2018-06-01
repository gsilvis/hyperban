# Set to DEBUG or RELEASE
#MODE = DEBUG
MODE = RELEASE

CFLAGS = -Wall -Wextra -std=gnu99
CFLAGS += -Wno-unused-parameter -Wno-abi -Wno-unused-function -Wno-unused-const-variable
ifeq ($(MODE),DEBUG)
	CFLAGS += -g
else
	CFLAGS += --closure 1 -O3
endif
CFLAGS += -Imodule

LDFLAGS = $(CFLAGS)
LDFLAGS += -lm
LDFLAGS += -lidbfs.js

EXPFUNCS = "['_js_draw_graph','_js_get_pos','_js_load_board','_js_dump_board','_js_do_move','_js_undo_move','_js_edit_board','_js_get_unsolved','_js_get_moves','_js_set_custom_projection','_js_init']"

EMCC_FLAGS = --js-library module/cairo.js
EMCC_FLAGS += -s EXPORTED_FUNCTIONS=$(EXPFUNCS)
ifeq ($(MODE),RELEASE)
	EMCC_FLAGS += --llvm-lto 1
endif
EMCC_FLAGS += --preload-file levels
EMCC_FLAGS += -s STRICT=1
EMCC_FLAGS += -s FORCE_FILESYSTEM=1
EMCC_FLAGS += -s MODULARIZE=1
EMCC_FLAGS += -s 'ENVIRONMENT="web"'
EMCC_FLAGS += -s EXPORT_NAME="'Hyperban'"
EMCC_FLAGS += -s WASM=0
EMCC_FLAGS += -s --pre-js module/cairo_pre.js
EMCC_FLAGS += -s 'EXTRA_EXPORTED_RUNTIME_METHODS=["FS","cwrap","_malloc","stringToUTF8","lengthBytesUTF8"]'

ifeq ($(MODE),DEBUG)
	WEBPACK_FLAGS += --mode=development
else
	WEBPACK_FLAGS += --mode=production
endif

CFILES = module/renderer.c $(wildcard module/gui/*.c) $(wildcard module/graph/*.c)
OFILES = $(patsubst %.c, %.o, $(CFILES))

JSFILES = web/glue.js \
	web/hyperban_component.js \
	web/renderer.js

.PHONY: all make-dist build-module build-web run-web

all: make-dist build-module build-web

make-dist:
	mkdir -p dist

build-module: web/renderer.js

build-web: dist/hyperban.js dist/index.html dist/renderer.data dist/renderer.js.mem

run-web: build-web
	./node_modules/.bin/http-server ./dist

dist/index.html: web/index.html
	cp $< $@

node_modules:
	npm install

dist/hyperban.js: node_modules webpack.config.js $(JSFILES)
	./node_modules/.bin/webpack $(WEBPACK_FLAGS)

web/renderer.js web/renderer.data web/renderer.js.mem: $(OFILES) module/cairo.js module/cairo_pre.js
	emcc $(EMCC_FLAGS) -o $@ $(filter %.o, $^) $(LDFLAGS)

dist/renderer.data: web/renderer.data
	cp $< $@

dist/renderer.js.mem: web/renderer.js.mem
	cp $< $@

%.o: %.c
	emcc -c -o $@ $^ $(CFLAGS) $(EMCC_FLAGS)

clean:
	rm -rf $(OFILES) \
		web/renderer.* \
		dist

CC=emcc
SRC=renderer.cpp
CFLAGS=-Werror -std=c++11 -O1 -g4
EMSCRIPTEN_FLAGS=-s USE_WEBGL2=1 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_FUNCTIONS='["_initializeOpenGL", "_render", "_loadJPEGImage"]'
RENDERER_NAME=renderer
OUTPUT=$(RENDERER_NAME).js

all: $(SRC) $(OUTPUT)

$(OUTPUT): $(SRC)
	$(CC) $(SRC) $(CFLAGS) $(EMSCRIPTEN_FLAGS) --post-js renderer_post.js -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)
	rm -f $(OUTPUT).mem
	rm -f $(OUTPUT).map
	rm -f $(RENDERER_NAME).wasm
	rm -f $(RENDERER_NAME).wasm.map
	rm -f $(RENDERER_NAME).wast
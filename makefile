CC=emcc
SRC=renderer.cpp
CFLAGS=-Werror -std=c++11 -O1 -g4
EMSCRIPTEN_FLAGS=-s USE_WEBGL2=1 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_FUNCTIONS='["_initializeOpenGL", "_render", "_loadJPEGImage"]'
OUTPUT=renderer.js

all: $(SRC) $(OUTPUT)

$(OUTPUT): $(SRC)
	$(CC) $(SRC) $(CFLAGS) $(EMSCRIPTEN_FLAGS) --post-js renderer_post.js -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)
	rm -f $(OUTPUT).mem
	rm -f $(OUTPUT).map
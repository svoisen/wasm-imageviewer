CC=emcc
SRC=renderer.cpp
CFLAGS=-Werror -std=c++11 -O2 -g0
EMSCRIPTEN_FLAGS=-s USE_WEBGL2=1
OUTPUT=renderer.js

all: $(SRC) $(OUTPUT)

$(OUTPUT): $(SRC)
	$(CC) $(SRC) $(CFLAGS) $(EMSCRIPTEN_FLAGS) --post-js renderer_post.js -o $(OUTPUT)

clean:
	rm $(OUTPUT)
	rm $(OUTPUT).mem
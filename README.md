# WASM Image Viewer

This repository provides an example of how to use C++, OpenGL and Emscripten to create a simple zoomable image viewer for the web using WebAssembly.

You can view this example in a desktop browser that supports WebAssembly here: https://svoisen.github.io/wasm-imageviewer/

## Building

To build this project you will need the Emscripten compiler (`emcc`) installed and accessible in your `$PATH`. You will also need the GNU `make` tool. If you haven't already installed Emscripten, follow [the instructions on the Emscripten website](https://kripken.github.io/emscripten-site/docs/getting_started/downloads.html).

Once you have everything installed, simply run `make` in the project directory, then open `index.html`. 

## Running

Due to cross-origin security issues, some browsers may require that you serve the contents of the project from a webserver. This is because the Emscripten runtime uses an XHR request to load the WebAssembly code. One simple option, if you have Python installed, is to use Python's SimpleHTTPServer:

```
python -m SimpleHTTPServer
```

Then open `http://localhost:8000` to view the example.
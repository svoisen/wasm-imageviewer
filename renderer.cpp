#include <iostream>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <SDL/SDL.h>

#include <emscripten.h>

SDL_Surface* screen;

extern "C" {

int EMSCRIPTEN_KEEPALIVE initializeOpenGL(int width, int height) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) == 0) {
    screen = SDL_SetVideoMode(width, height, 0, SDL_OPENGL);
  } else {
    std::cerr << "Error initializing SDL: " << SDL_GetError() << std::endl;
    return 0;
  }

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glViewport(0, 0, width, height);

  return 1;
}

}
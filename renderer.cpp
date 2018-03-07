#include <iostream>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <SDL2/SDL.h>
#include <emscripten.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window *window;
SDL_GLContext glContext;
GLuint program;
GLuint texture;
int textureWidth;
int textureHeight;
bool textureLoaded = false;

GLfloat planeVertices[] = {
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f
};

const char vertexShaderSrc[] =
    "#version 300 es                                                          \n"
    "uniform float zoom;                                                      \n"
    "uniform float aspect;                                                    \n"
    "in vec4 vert;                                                            \n"
    "out vec2 texCoord;                                                       \n"
    "void main() {                                                            \n"
    "   gl_Position = vec4(vert.x * zoom, vert.y * aspect * zoom, 0.0f, 1.0f);\n"
    "   texCoord = vec2((vert.x + 1.0f) / 2.0f, (1.0f - vert.y) / 2.0);       \n"
    "}                                                                        \n";

const char fragmentShaderSrc[] =
    "#version 300 es                        \n"
    "precision mediump float;               \n"
    "uniform sampler2D tex;                 \n"
    "in vec2 texCoord;                      \n"
    "out vec4 finalColor;                   \n"
    "void main() {                          \n"
    "   finalColor = texture(tex, texCoord);\n"
    "}                                      \n";

GLuint compileShader(GLenum type, const char *source) {
    GLuint shader = glCreateShader(type);
    if (shader == 0) {
        std::cerr << "Error creating shader" << std::endl;
        return 0;
    }

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint shaderCompiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);
    if (!shaderCompiled) {
        std::cerr << "Error compiling shader" << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint buildProgram(GLuint vertexShader, GLuint fragmentShader, const char *vertexPositionName) {
    GLuint programObject = glCreateProgram();
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);
    glBindAttribLocation(programObject, 0, vertexPositionName);
    glLinkProgram(programObject);

    GLint programLinked;
    glGetProgramiv(programObject, GL_LINK_STATUS, &programLinked);
    if (!programLinked) {
        std::cerr << "Error linking program" << std::endl;
        glDeleteProgram(programObject);
        return 0;
    }

    return programObject;
}

extern "C" {

int initializeOpenGL(int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == 0) {
        window = SDL_CreateWindow("Image Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);
    } else {
        std::cerr << "Error initializing SDL: " << SDL_GetError() << std::endl;
        return 0;
    }

    glContext = SDL_GL_CreateContext(window);
    SDL_EventState(SDL_MOUSEWHEEL, SDL_IGNORE);

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
    program = buildProgram(vertexShader, fragmentShader, "vert");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, width, height);

    return 1;
}

void render(float zoom) {
    glClear(GL_COLOR_BUFFER_BIT);

    if (!textureLoaded) {
        std::cout << "No image to render" << std::endl;
        return;
    }

    std::cout << "Rendering image" << std::endl;
    glUseProgram(program);

    // Setup VBO
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(GLfloat), planeVertices, GL_STATIC_DRAW);

    // Setup VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    GLint vertexPositionAttribute = glGetAttribLocation(program, "vert");
    glBindVertexArray(vao);
    glEnableVertexAttribArray(vertexPositionAttribute);
    glVertexAttribPointer(vertexPositionAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Bind to texture
    GLuint textureUniform = glGetUniformLocation(program, "tex");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(textureUniform, 0);

    // Set zoom
    GLuint zoomUniform = glGetUniformLocation(program, "zoom");
    glUniform1f(zoomUniform, zoom);

    // Set aspect ratio
    GLuint aspectUniform = glGetUniformLocation(program, "aspect");
    glUniform1f(aspectUniform, (float)textureHeight/(float)textureWidth);

    // Draw
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Unbind
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    SDL_GL_SwapWindow(window);
}

int loadJPEGImage(uint8_t* buffer, size_t size) {
    // Delete any existing textures before proceeding
    if (textureLoaded) {
        glDeleteTextures(1, &texture);
        textureLoaded = false;
    }

    int channels;
    uint8_t* imageData = stbi_load_from_memory(buffer, static_cast<int>(size), &textureWidth, &textureHeight, &channels, STBI_rgb_alpha);
    if (!imageData) {
        std::cerr << "Error loading image" << std::endl;
        return 0;
    } 

    std::cout << "Loaded image with dimensions " << textureWidth << "x" << textureHeight << ", size " << size << ", and channels " << channels << std::endl;

    // Generate texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(imageData);
    textureLoaded = true;
    return 1;
}

}
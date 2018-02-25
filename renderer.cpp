#include <iostream>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <SDL/SDL.h>

#include <emscripten.h>

SDL_Surface *screen;

GLuint program;

GLfloat planeVertices[] = {
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f
};

const char vertexShaderSrc[] =
    "#version 300 es                        \n"
    "in vec4 vert;                          \n"
    "out vec3 color;                        \n"
    "void main() {                          \n"
    "   gl_Position = vert;                 \n"
    "   color = gl_Position.xyz + vec3(0.5);\n"
    "}                                      \n";

const char fragmentShaderSrc[] =
    "#version 300 es                        \n"
    "precision mediump float;               \n"
    "in vec3 color;                         \n"
    "out vec4 finalColor;                   \n"
    "void main() {                          \n"
    "   finalColor = vec4(color, 1.0);      \n"
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
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) == 0) {
        screen = SDL_SetVideoMode(width, height, 0, SDL_OPENGL);
    } else {
        std::cerr << "Error initializing SDL: " << SDL_GetError() << std::endl;
        return 0;
    }

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
    program = buildProgram(vertexShader, fragmentShader, "vert");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, width, height);

    return 1;
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);

    GLuint vbo;
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(GLfloat), planeVertices, GL_STATIC_DRAW);

    GLint vertexPositionAttribute = glGetAttribLocation(program, "vert");
    glBindVertexArray(vao);
    glEnableVertexAttribArray(vertexPositionAttribute);
    glVertexAttribPointer(vertexPositionAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    SDL_GL_SwapBuffers();
}

}
#ifndef RENDER_H
#define RENDER_H

#include "glad/gl.h"
#include <SDL3/SDL.h>

// Render functions
int createShaderProgram();
int loadTexture(const char *path);
void drawQuad(int shaderProgram, int texture);

#endif
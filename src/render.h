#ifndef RENDER_H
#define RENDER_H

#include "glad/gl.h"
#include <SDL3/SDL.h>

typedef int shader_t;
typedef int texture_t;

// Render functions
shader_t shader_compile(const char *vertex_path, const char *fragment_path);
texture_t texture_load(const char *path);
void draw_quad(int shader_program, int texture, float x, float y, float width, float height);

#endif

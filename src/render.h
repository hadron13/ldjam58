#ifndef RENDER_H
#define RENDER_H

#include "glad/gl.h"
#include <SDL3/SDL.h>

// Render functions
int create_shader_program();
int load_texture(const char *path);
void draw_quad(int shader_program, int texture);

#endif
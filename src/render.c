#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "render.h"
#include <stdio.h>

// TODO: hardcode the shaders
static const char *vertex_shader = "";
static const char *fragment_shader = "";


shader_t shader_compile(const char *vertex_path, const char *fragment_path){ 
    return 0;
}

texture_t texture_load(const char *path) {
    return 0;
}

void draw_quad(shader_t shader, texture_t texture) {

}

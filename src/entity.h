#ifndef ENTITY_H
#define ENTITY_H

#include "render.h"

#define MAX_ENTITIES 64

typedef struct {
    float x, y;
    float w, h;
    float c_radius;
    int texture;
    int normal_texture;
    float r;
} sprite_t;

void render_entities(const sprite_t sprites[MAX_ENTITIES], int count, int shader_program, float window_w, float window_h, float camera_pos_x, float camera_pos_y);
int is_colliding(const sprite_t sprites[MAX_ENTITIES], int count);

#endif

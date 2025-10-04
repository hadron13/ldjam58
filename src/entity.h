#ifndef ENTITY_H
#define ENTITY_H

#include "render.h"

#define MAX_ENTITIES 64

typedef struct {
    float x, y;
    float w, h;
    int texture;
} sprite_t;

void render_entities(const sprite_t sprites[MAX_ENTITIES], int count, int shader_program);

#endif
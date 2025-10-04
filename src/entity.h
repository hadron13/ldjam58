#ifndef ENTITY_H
#define ENTITY_H

#define MAX_ENTITIES 64

typedef struct {
    float x, y;
    float w, h;
    int texture;
} sprite;

void render_entities(const sprite sprites[MAX_ENTITIES], int count, int shader_program);

#endif
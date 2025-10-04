#include "entity.h"

void render_entities(const sprite_t sprites[MAX_ENTITIES], int count, int shader_program) {
    for (int i = 0; i < count; ++i) {
        draw_quad(shader_program, sprites[i].texture, sprites[i].x, sprites[i].y, sprites[i].w, sprites[i].h);
    }
}
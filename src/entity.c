#include "entity.h"

void render_entities(const sprite_t sprites[MAX_ENTITIES], int count, int shader_program, float window_w, float window_h, float camera_pos_x, float camera_pos_y) {
    for (int i = 0; i < count; ++i) {
        draw_quad(shader_program, sprites[i].texture, sprites[i].normal_texture, sprites[i].x - camera_pos_x, sprites[i].y - camera_pos_y, sprites[i].w, sprites[i].h, sprites[i].r, window_w, window_h);
    }
}

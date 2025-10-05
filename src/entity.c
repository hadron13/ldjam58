#include "entity.h"

void render_entities(const sprite_t sprites[MAX_ENTITIES], int count, int shader_program, float window_w, float window_h, float camera_pos_x, float camera_pos_y) {
    for (int i = 0; i < count; ++i) {
        draw_quad(shader_program, sprites[i].texture, sprites[i].normal_texture, sprites[i].x - camera_pos_x, sprites[i].y - camera_pos_y, sprites[i].w, sprites[i].h, sprites[i].r, window_w, window_h);
    }
}

int is_colliding(const sprite_t sprites[MAX_ENTITIES], int count) {
    for (int i = 2; i < count; ++i) {
        float radius = sprites[i].c_radius;
        vec2 pos = {sprites[i].x + sprites[i].w / 2, sprites[i].y + sprites[i].h / 2};
        vec2 rocket_pos = {sprites[1].x + sprites[1].w / 2, sprites[1].y + sprites[1].h / 2};
        if (glm_vec2_distance(pos, rocket_pos) < radius) return true;
    }

    return false;
}
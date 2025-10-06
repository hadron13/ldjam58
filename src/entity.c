#include "entity.h"

void render_entities(const sprite_t sprites[MAX_ENTITIES], int count, int shader_program, float window_w, float window_h, float camera_pos_x, float camera_pos_y, float camera_scale) {
    for (int i = 0; i < count; ++i) {
        if (sprites[i].visible) draw_quad(shader_program, sprites[i].texture, sprites[i].normal_texture, sprites[i].x - camera_pos_x, sprites[i].y - camera_pos_y, sprites[i].w * camera_scale, sprites[i].h * camera_scale, sprites[i].r, sprites[i].alpha, window_w, window_h);
    }
}

static inline void rotate_point(float *x, float *y, float angle) {
    float cs = cosf(angle);
    float sn = sinf(angle);
    float px = *x, py = *y;
    *x = px * cs - py * sn;
    *y = px * sn + py * cs;
}

int is_colliding(const sprite_t sprites[MAX_ENTITIES], int count) {
    float rocket_cx = sprites[1].x + sprites[1].w / 2.0f;
    float rocket_cy = sprites[1].y + sprites[1].h / 2.0f;
    float half_w = sprites[1].c_w / 2.0f;
    float half_h = sprites[1].c_h / 2.0f;
    float angle = sprites[1].r; // in radians

    for (int i = 2; i < count; ++i) {
        float circle_x = sprites[i].x + sprites[i].w / 2.0f;
        float circle_y = sprites[i].y + sprites[i].h / 2.0f;
        float radius   = sprites[i].c_radius;

        float local_x = circle_x - rocket_cx;
        float local_y = circle_y - rocket_cy;
        rotate_point(&local_x, &local_y, -angle);

        float closest_x = fmaxf(-half_w, fminf(local_x, half_w));
        float closest_y = fmaxf(-half_h, fminf(local_y, half_h));

        float dx = local_x - closest_x;
        float dy = local_y - closest_y;
        float dist_sq = dx*dx + dy*dy;

        if (dist_sq < radius * radius) {
            return i;
        }
    }

    return 0;
}

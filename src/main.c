#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_scancode.h>
#include"glad/gl.h"
#include"render.h"
#include"entity.h"
#include"audio.h"
#define GLT_IMPLEMENTATION
#include"gl_text.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

sprite_t sprites[MAX_ENTITIES];
sprite_t rocket_flame;
int viewport_w = 800, viewport_h = 600;

float camera_pos_x = 0.0, camera_pos_y = 0.0;

int sprite_shader;
int background_shader;
int RCS_id;
int background_id;
float rocket_acc_x = 0, rocket_acc_y = 0;
float rocket_radial_acc = 0;
float rocket_fuel = 100.0;
float speed = 2.0f;
double timer = 0.0;
char str[64];
char str2[64];
char str3[64];
GLTtext *text1;
GLTtext *text2;
GLTtext *text3;

int engine_on = false;

int asteroid_albedo_textures[3];
int asteroid_normal_textures[3];

int tank_albedo_texture;
int tank_normal_texture;

typedef struct {
    float vx, vy;
    int active;
} asteroid_data_t;

typedef struct {
    float vx, vy;
    int active;
} tanks_data_t;

#define MAX_TANKS 5
#define MAX_ASTEROIDS MAX_ENTITIES-MAX_TANKS

asteroid_data_t asteroid_data[MAX_ASTEROIDS];
tanks_data_t tanks_data[MAX_TANKS];

int asteroid_count = 0;
int tank_count = 0;

SDL_AudioSpec background_spec;
SDL_AudioStream *background_stream;

SDL_AudioSpec RCS_spec;
SDL_AudioStream *RCS_stream;

void spawn_asteroid() {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (!asteroid_data[i].active && i != 1) {
            float angle = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
            float dist = 1000.0f + ((float)rand() / RAND_MAX) * 500.0f;
            float px = sprites[1].x + cosf(angle) * dist;
            float py = sprites[1].y + sinf(angle) * dist;
            float size = rand() % 200;
            float w = 100 + size;
            float h = 100 + size;
            int rand_num = rand() % 3;
            int asteroid_albedo_texture = asteroid_albedo_textures[rand_num];
            int asteroid_normal_texture = asteroid_normal_textures[rand_num];
            sprites[i] = (sprite_t){px, py, w, h, 0, 0, w / 2.86f, 1.0, asteroid_albedo_texture, asteroid_normal_texture};
            asteroid_data[i].vx = rocket_acc_x;
            asteroid_data[i].vy = rocket_acc_y;
            asteroid_data[i].active = 1;
            asteroid_count++;
            break;
        }
    }
}

void disable_asteroids() {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        asteroid_data[i].active = false;
        asteroid_count = 0;
    }
}

void update_asteroids(float dt) {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (asteroid_data[i].active && i != 1) {
            sprites[i].x += asteroid_data[i].vx * dt;
            sprites[i].y += asteroid_data[i].vy * dt;
            float dx = sprites[i].x - sprites[1].x;
            float dy = sprites[i].y - sprites[1].y;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist > 2500.0f) {
                asteroid_data[i].active = 0;
                asteroid_count--;
            }
        }
    }
    if (asteroid_count < 10) {
        if (rand() % 100 < 5) {
            spawn_asteroid();
        }
    }
}

void spawn_tanks() {
    for (int i = 0; i < MAX_TANKS; i++) {
        if (!tanks_data[i].active && i != 1) {
            float angle = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
            float dist = 1000.0f + ((float)rand() / RAND_MAX) * 500.0f;
            float px = sprites[1].x + cosf(angle) * dist;
            float py = sprites[1].y + sinf(angle) * dist;
            float size = rand() % 200;
            float w = 100 + size;
            float h = 100 + size;
            int rand_num = rand() % 3;
            sprites[i] = (sprite_t){px, py, w, h, 0, 0, w / 2.86f, 1.0, tank_albedo_texture, tank_normal_texture};
            tanks_data[i].vx = 0.0;
            tanks_data[i].vy = 0.0;
            tanks_data[i].active = 1;
            tank_count++;
            break;
        }
    }
}

void disable_tanks() {
    for (int i = 0; i < MAX_TANKS; i++) {
        tanks_data[i].active = false;
        tank_count = 0;
    }
}

void update_tanks(float dt) {
    for (int i = 0; i < MAX_TANKS; i++) {
        if (tanks_data[i].active && i != 1) {
            sprites[i].x += tanks_data[i].vx * dt;
            sprites[i].y += tanks_data[i].vy * dt;
            float dx = sprites[i].x - sprites[1].x;
            float dy = sprites[i].y - sprites[1].y;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist > 2500.0f) {
                tanks_data[i].active = 0;
                tank_count--;
            }
        }
    }
    if (tank_count < 2) {
        if (rand() % 100 < 5) {
            spawn_tanks();
        }
    }
}

void menu_state(float dt, int *current_state) {
    const bool* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_SPACE]) *current_state = 1;

    glClearColor(0.1, 0.1, 0.1, 1.0); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_quad(background_shader, 0, 0, 0, 0, viewport_w, viewport_h, 0, 0, viewport_w, viewport_h);

    gltBeginDraw();

    gltSetText(text1, "Press SPACE to Start Game");
    
    gltColor(1.0f, 1.0f, 1.0f, 1.0f);
    gltDrawText2DAligned(text1, viewport_w / 2.0f, viewport_h / 2.0f, 1.0f, GLT_CENTER, GLT_CENTER);

    gltEndDraw();
}

void game_state(float dt, int *current_state) {
    const bool* keys = SDL_GetKeyboardState(NULL);

    float cos_r = cosf(sprites[1].r);
    float sin_r = sinf(sprites[1].r);

    if (keys[SDL_SCANCODE_W]) {
        rocket_acc_x += cos_r * speed * dt;
        rocket_acc_y += sin_r * speed * dt;
    }
    if (keys[SDL_SCANCODE_S]) {
        rocket_acc_x -= cos_r * speed * dt;
        rocket_acc_y -= sin_r * speed * dt;
    }
    if (keys[SDL_SCANCODE_A]) {
        rocket_acc_x += sin_r * speed * dt;
        rocket_acc_y += -cos_r * speed * dt;
    }
    if (keys[SDL_SCANCODE_D]) {
        rocket_acc_x += -sin_r * speed * dt;
        rocket_acc_y += cos_r * speed * dt;
    }

    if (keys[SDL_SCANCODE_Q]) rocket_radial_acc -= (speed / 90.0f) * dt;
    if (keys[SDL_SCANCODE_E]) rocket_radial_acc += (speed / 90.0f) * dt;

    if ((keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_A]
    || keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_Q] || keys[SDL_SCANCODE_E]) && rocket_fuel >= 0.0) {
        resume_sound(RCS_id, &RCS_stream);
        rocket_fuel -= dt;
    }
    else pause_sound(RCS_id, &RCS_stream);

    if (engine_on) {
        rocket_acc_x += cos_r * speed * 10.0f * dt;
        rocket_acc_y += sin_r * speed * 10.0f * dt;

        rocket_fuel -= dt * 0.5;
    }

    glm_clamp(rocket_fuel, 0.0, 100.0);

    update_audio(RCS_id, &RCS_stream, &RCS_spec);
    update_audio(background_id, &background_stream, &background_spec);

    sprites[1].x += rocket_acc_x; sprites[1].y += rocket_acc_y;
    sprites[1].r += rocket_radial_acc;
    //sprites[5].r += rocket_radial_acc;

    float flame_offset = (sprites[1].h / 2.0f) + (rocket_flame.h / 2.0f) - 45.0f;

    float center_x = sprites[1].x + sprites[1].w / 2.0f;
    float center_y = sprites[1].y + sprites[1].h / 2.0f;

    float local_x = -flame_offset;
    float local_y = 0.0f;

    rocket_flame.x = center_x + cosf(sprites[1].r) * local_x - sinf(sprites[1].r) * local_y - rocket_flame.w / 2.0f;
    rocket_flame.y = center_y + sinf(sprites[1].r) * local_x + cosf(sprites[1].r) * local_y - rocket_flame.h / 2.0f;
    rocket_flame.r = sprites[1].r - 1.57f;

    update_asteroids(dt);
    update_tanks(dt);

    glClearColor(0.1, 0.1, 0.1, 1.0); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_quad(background_shader, 0, 0, 0, 0, viewport_w, viewport_h, 0, 0, viewport_w, viewport_h);
    
    render_entities(sprites, MAX_ENTITIES, sprite_shader, viewport_w, viewport_h, camera_pos_x, camera_pos_y);

    if (engine_on && rocket_fuel >= 0.0){ 
        rocket_flame.alpha = glm_lerp(rocket_flame.alpha,1.0, 0.01);
        draw_quad(sprite_shader, rocket_flame.texture, rocket_flame.normal_texture, rocket_flame.x - camera_pos_x, rocket_flame.y - camera_pos_y, rocket_flame.w, rocket_flame.h, rocket_flame.r, rocket_flame.alpha, viewport_w, viewport_h);
    }else{
        rocket_flame.alpha = 0.0;
    }
    int is_rocket_colliding = is_colliding(sprites, MAX_ENTITIES);

    camera_pos_x = sprites[1].x - viewport_w / 2.0f + 256.0f; camera_pos_y = sprites[1].y - viewport_h / 2.0f + 256.0f;

    if (is_rocket_colliding) {
        sprites[1].x = viewport_w / 2.0;
        sprites[1].y = viewport_h / 2.0;
        rocket_acc_x = 0.0;
        rocket_acc_y = 0.0;
        rocket_radial_acc = 0.0;
        disable_asteroids();
        disable_tanks();
        *current_state = 2;
    }
        
    gltBeginDraw();

    sprintf(str, "rocket vel: %.4f | radial vel: %.4f", fabs(rocket_acc_x + rocket_acc_y), fabs(rocket_radial_acc));
    sprintf(str2, "fuel remaining: %.1f", rocket_fuel);
    sprintf(str3, "asteroids: %d", asteroid_count);
    gltSetText(text2, str);
    gltSetText(text1, str2);
    gltSetText(text3, str3);
    
    gltColor(1.0f, 1.0f, 1.0f, 1.0f);
    gltDrawText2D(text1, 0.0f, 0.0f, 1.0f);
    gltDrawText2D(text3, 0.0f, 20.0f, 1.0f);

    gltColor(
        cosf((float)timer) * 0.5f + 0.5f,
        sinf((float)timer) * 0.5f + 0.5f,
        1.0f,
        1.0f);

    gltDrawText2DAligned(text2, 0.0f, (GLfloat)viewport_h, 1.0f, GLT_LEFT, GLT_BOTTOM);

    gltEndDraw();

    timer += dt;
}

void game_over_state(float dt, int *current_state) {
    const bool* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_SPACE]) *current_state = 0;

    glClearColor(0.1, 0.1, 0.1, 1.0); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_quad(background_shader, 0, 0, 0, 0, viewport_w, viewport_h, 0, 1.0, viewport_w, viewport_h);

    gltBeginDraw();

    gltSetText(text1, "Game Over! Press SPACE to Menu");
    
    gltColor(1.0f, 1.0f, 1.0f, 1.0f);
    gltDrawText2DAligned(text1, viewport_w / 2.0f, viewport_h / 2.0f, 1.0f, GLT_CENTER, GLT_CENTER);

    gltEndDraw();
}

int main(){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
    srand(time(NULL));

    init_audio(&background_spec, &background_stream);
    init_audio(&RCS_spec, &RCS_stream);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window * window = SDL_CreateWindow("Garbage Collection !!!", viewport_w, viewport_h, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

    
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    if(!SDL_GL_SetSwapInterval(-1)){
        SDL_GL_SetSwapInterval(1);
    }
    
    gladLoadGL(SDL_GL_GetProcAddress);

    gltInit();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    
    sprite_shader = shader_compile("assets/shaders/quad.vert.glsl", "assets/shaders/sprite.frag.glsl");    
    background_shader = shader_compile("assets/shaders/quad.vert.glsl", "assets/shaders/background.frag.glsl");

    int asteroid_albedo_texture1 = texture_load("assets/images/asteroid_albedo.png");
    int asteroid_normal_texture1 = texture_load("assets/images/asteroid_normal.png");
    int asteroid_albedo_texture2 = texture_load("assets/images/asteroid2_albedo.png");
    int asteroid_normal_texture2 = texture_load("assets/images/asteroid2_normal.png");
    int asteroid_albedo_texture3 = texture_load("assets/images/asteroid3_albedo.png");
    int asteroid_normal_texture3 = texture_load("assets/images/asteroid3_normal.png");

    int rocket_albedo_texture = texture_load("assets/images/rocket_albedo.png");
    int rocket_normal_texture = texture_load("assets/images/rocket_normal.png");
    int flame_albedo_texture = texture_load("assets/images/rocketflame.png");

    tank_albedo_texture = texture_load("assets/images/tank_albedo.png");
    tank_normal_texture = texture_load("assets/images/tank_normal.png");

    asteroid_albedo_textures[0] = asteroid_albedo_texture1;
    asteroid_albedo_textures[1] = asteroid_albedo_texture2;
    asteroid_albedo_textures[2] = asteroid_albedo_texture3;

    asteroid_normal_textures[0] = asteroid_normal_texture1;
    asteroid_normal_textures[1] = asteroid_normal_texture2;
    asteroid_normal_textures[2] = asteroid_normal_texture3;

    background_id = load_sound("assets/sfx/space.wav", &background_spec);
    RCS_id = load_sound("assets/sfx/RCS.wav", &RCS_spec);

    rocket_flame = (sprite_t){viewport_w / 2.0, viewport_h / 2.0, 150, 500, 0, 0, 0, 1.0, flame_albedo_texture};
    sprites[1] = (sprite_t){viewport_w / 2.0, viewport_h / 2.0, 512, 512, 440, 105, 75, 1.0, rocket_albedo_texture, rocket_normal_texture};
    //sprites[5] = (sprite_t){800, 100, 200, 200, 0, 0, 70, 1.0, asteroid_albedo_texture, asteroid_normal_texture};

    bool running = true;

    play_sound(background_id, 1, &background_stream);
    play_sound(RCS_id, 1, &RCS_stream);
    pause_sound(RCS_id, &RCS_stream);
    
    text1 = gltCreateText();
    text2 = gltCreateText();
    text3 = gltCreateText();

    Uint32 last_time = SDL_GetTicks();

    int current_state = 0;
    
    while(running){
        Uint32 current_time = SDL_GetTicks();
        float dt = (current_time - last_time) / 1000.0f;
        last_time = current_time;
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                    viewport_w = event.window.data1;
                    viewport_h = event.window.data2;
                    glViewport(0, 0, viewport_w, viewport_h);
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (!event.key.repeat) {
                        if (event.key.key == SDLK_LSHIFT) {
                            engine_on = true;
                        }
                    }
                    break;
                case SDL_EVENT_KEY_UP:
                    if (!event.key.repeat) {
                        if (event.key.key == SDLK_LSHIFT) {
                            engine_on = false;
                        }
                    }
                    break;
            }
        }

        switch(current_state) {
            case 0: {
                menu_state(dt, &current_state);
                break;
            }
            case 1: {
                game_state(dt, &current_state);
                break;
            }
            case 2: {
                game_over_state(dt, &current_state);
                break;
            }
        }

        SDL_GL_SwapWindow(window);
    }

    gltDeleteText(text2);
    gltTerminate();
    
    SDL_GL_DestroyContext(gl_context);

    SDL_DestroyWindow(window); 
    SDL_Quit();
}

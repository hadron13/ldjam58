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

sprite_t sprites[MAX_ENTITIES];
int viewport_w = 800, viewport_h = 600;

float camera_pos_x = 0.0, camera_pos_y = 0.0;

int sprite_shader;
int background_shader;
int RCS_id;
float rocket_acc_x = 0, rocket_acc_y = 0;
float rocket_radial_acc = 0;
float speed = 2.0f;
double time = 0.0;
char str[64];
char str2[64];
GLTtext *text1;
GLTtext *text2;

void menu_state(float dt, int *current_state) {
    const bool* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_SPACE]) *current_state = 1;

    glClearColor(0.1, 0.1, 0.1, 1.0); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_quad(background_shader, 0, 0, 0, 0, viewport_w, viewport_h, 0, viewport_w, viewport_h);

    gltBeginDraw();

    gltSetText(text1, "Press SPACE to Start Game");
    
    gltColor(1.0f, 1.0f, 1.0f, 1.0f);
    gltDrawText2DAligned(text1, viewport_w / 2.0f, viewport_h / 2.0f, 1.0f, GLT_CENTER, GLT_CENTER);

    gltEndDraw();
}

void game_state(float dt, int *current_state) {
    const bool* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_W]) rocket_acc_y -= speed * dt;
    if (keys[SDL_SCANCODE_S]) rocket_acc_y += speed * dt;
    if (keys[SDL_SCANCODE_A]) rocket_acc_x -= speed * dt;
    if (keys[SDL_SCANCODE_D]) rocket_acc_x += speed * dt;

    if (keys[SDL_SCANCODE_Q]) rocket_radial_acc -= (speed / 90.0f) * dt;
    if (keys[SDL_SCANCODE_E]) rocket_radial_acc += (speed / 90.0f) * dt;

    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_A]
    || keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_Q] || keys[SDL_SCANCODE_E]) resume_sound(RCS_id);
    else pause_sound(RCS_id);

    update_audio();

    sprites[1].x += rocket_acc_x; sprites[1].y += rocket_acc_y;
    sprites[1].r += rocket_radial_acc;

    glClearColor(0.1, 0.1, 0.1, 1.0); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_quad(background_shader, 0, 0, 0, 0, viewport_w, viewport_h, 0, viewport_w, viewport_h);
    
    render_entities(sprites, 6, sprite_shader, viewport_w, viewport_h, camera_pos_x, camera_pos_y);
    int is_rocket_colliding = is_colliding(sprites, 6);

    camera_pos_x = sprites[1].x - viewport_w / 2.0f + 50.0f; camera_pos_y = sprites[1].y - viewport_h / 2.0f + 50.0f;

    if (is_rocket_colliding) {
        sprites[1].x = viewport_w / 2.0;
        sprites[1].y = viewport_h / 2.0;
        *current_state = 2;
    }
        
    gltBeginDraw();

    sprintf(str, "rocket accel: %.4f | radial accel: %.4f", fabs(rocket_acc_x + rocket_acc_y), fabs(rocket_radial_acc));
    sprintf(str2, "rocket colliding: %i", is_rocket_colliding);
    gltSetText(text2, str);
    gltSetText(text1, str2);
    
    gltColor(1.0f, 1.0f, 1.0f, 1.0f);
    gltDrawText2D(text1, 0.0f, 0.0f, 1.0f);

    gltColor(
        cosf((float)time) * 0.5f + 0.5f,
        sinf((float)time) * 0.5f + 0.5f,
        1.0f,
        1.0f);

    gltDrawText2DAligned(text2, 0.0f, (GLfloat)viewport_h, 1.0f, GLT_LEFT, GLT_BOTTOM);

    gltEndDraw();

    time += dt;
}

void game_over_state(float dt, int *current_state) {
    const bool* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_SPACE]) *current_state = 0;

    glClearColor(0.1, 0.1, 0.1, 1.0); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_quad(background_shader, 0, 0, 0, 0, viewport_w, viewport_h, 0, viewport_w, viewport_h);

    gltBeginDraw();

    gltSetText(text1, "Game Over! Press SPACE to Menu");
    
    gltColor(1.0f, 1.0f, 1.0f, 1.0f);
    gltDrawText2DAligned(text1, viewport_w / 2.0f, viewport_h / 2.0f, 1.0f, GLT_CENTER, GLT_CENTER);

    gltEndDraw();
}

int main(){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);

    if (init_audio() < 0) {
        printf("Audio initialization failed\n");
    }

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

    int background_texture = texture_load("assets/images/Space_Background1.png");
    int meteor0_texture = texture_load("assets/images/mETEOR1.png");
    int meteor1_texture = texture_load("assets/images/mETEOR2.png");
    int meteor2_texture = texture_load("assets/images/mETEOR3.png");
    int meteor3_texture = texture_load("assets/images/mETEOR4.png");
    int asteroid_albedo_texture = texture_load("assets/images/asteroid_albedo.png");
    int asteroid_normal_texture = texture_load("assets/images/asteroid_normal.png");
    int rocket_albedo_texture = texture_load("assets/images/rocket_albedo.png");
    int rocket_normal_texture = texture_load("assets/images/rocket_normal.png");
    int placeholder_texture = texture_load("test.jpg");

    int sound_id = load_sound("assets/sfx/space.wav");
    if (sound_id < 0) printf("couldn't load the sound file\n");

    RCS_id = load_sound("assets/sfx/RCS.wav");

    sprites[1] = (sprite_t){viewport_w / 2.0, viewport_h / 2.0, 192, 108, 120, 40, 75, rocket_albedo_texture, rocket_normal_texture};
    //sprites[2] = (sprite_t){100, 150, 50, 50, 0, 0, 25, meteor0_texture};
    //sprites[3] = (sprite_t){300, 500, 100, 50, 0, 0, 25, meteor1_texture};
    //sprites[4] = (sprite_t){500, 300, 50, 50, 0, 0, 25, meteor2_texture};
    sprites[5] = (sprite_t){800, 100, 200, 200, 0, 0, 70, asteroid_albedo_texture, asteroid_normal_texture};

    bool running = true;

    play_sound(sound_id, 1);
    play_RCS_sound(RCS_id);
    
    text1 = gltCreateText();
    text2 = gltCreateText();

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
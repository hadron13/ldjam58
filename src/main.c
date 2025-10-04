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

    
    int sprite_shader = shader_compile("assets/shaders/quad.vert.glsl", "assets/shaders/sprite.frag.glsl");    
    int earth_shader = shader_compile("assets/shaders/quad.vert.glsl", "assets/shaders/earth.frag.glsl");

    int background_texture = texture_load("assets/images/Space_Background1.png");
    int placeholder_texture = texture_load("test.jpg");

    // main audio track
    int sound_id = load_sound("assets/sfx/space.wav");
    if (sound_id < 0) printf("couldn't load the sound file\n");

    // creating an entity
    sprites[0] = (sprite_t){0, 0, viewport_w, viewport_h, background_texture};
    sprites[1] = (sprite_t){viewport_w / 2, viewport_h / 2, 100, 100, placeholder_texture}; // rocket
    sprites[2] = (sprite_t){100, 150, 50, 50, placeholder_texture};
    sprites[3] = (sprite_t){300, 500, 50, 50, placeholder_texture};
    sprites[4] = (sprite_t){500, 300, 50, 50, placeholder_texture};

    float rocket_acc_x = 0, rocket_acc_y = 0;

    bool running = true;

    play_sound(sound_id);
    
    // text
    GLTtext *text1 = gltCreateText();
    gltSetText(text1, "Hello World!");

    GLTtext *text2 = gltCreateText();

    double time;
    char str[30];

    Uint32 last_time = SDL_GetTicks();
    float speed = 20.0f;
    
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

        const bool* keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_W]) rocket_acc_y += speed * dt;
        if (keys[SDL_SCANCODE_S]) rocket_acc_y -= speed * dt;
        if (keys[SDL_SCANCODE_A]) rocket_acc_x -= speed * dt;
        if (keys[SDL_SCANCODE_D]) rocket_acc_x += speed * dt;

        sprites[1].x += rocket_acc_x; sprites[1].y += rocket_acc_y;
        sprites[0].x = camera_pos_x; sprites[0].y = camera_pos_y;
        sprites[0].w = viewport_w; sprites[0].h = viewport_h;

        glClearColor(0.1, 0.1, 0.1, 1.0); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render_entities(sprites, 5, sprite_shader, viewport_w, viewport_h, camera_pos_x, camera_pos_y);

        camera_pos_x = sprites[1].x - viewport_w / 2.0f; camera_pos_y = sprites[1].y - viewport_h / 2.0f;
        
        // glt example code
        gltBeginDraw();

        gltColor(1.0f, 1.0f, 1.0f, 1.0f);
        gltDrawText2D(text1, 0.0f, 0.0f, 1.0f); // x=0.0, y=0.0, scale=1.0

        sprintf(str, "rocket accel: %.4f", fabs(rocket_acc_x + rocket_acc_y));
        gltSetText(text2, str);

        gltColor(
            cosf((float)time) * 0.5f + 0.5f,
            sinf((float)time) * 0.5f + 0.5f,
            1.0f,
            1.0f);

        gltDrawText2DAligned(text2, 0.0f, (GLfloat)viewport_h, 1.0f, GLT_LEFT, GLT_BOTTOM);

        gltEndDraw();
        // --

        SDL_GL_SwapWindow(window);
    }

    gltDeleteText(text1);
    gltDeleteText(text2);
    gltTerminate();
    
    SDL_GL_DestroyContext(gl_context);

    SDL_DestroyWindow(window); 
    SDL_Quit();
}

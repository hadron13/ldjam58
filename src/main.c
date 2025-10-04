#include<SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>
#include"glad/gl.h"
#include"render.h"
#include"entity.h"
#include<stdio.h>

sprite_t sprites[MAX_ENTITIES];

int main(){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);


    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window * window = SDL_CreateWindow("Garbage Collection !!!", 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

    
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    if(!SDL_GL_SetSwapInterval(-1)){
        SDL_GL_SetSwapInterval(1);
    }
    
    gladLoadGL(SDL_GL_GetProcAddress);

    // creating shader program and loading texture
    //                   shader path: using placeholder
    
    int shader_program = shader_compile("assets/shaders/quad.vert.glsl", "assets/shaders/earth.frag.glsl");
    if (!shader_program) printf("couldn't create shader program\n");

    int placeholder_texture = texture_load("test.jpg");
    if (!placeholder_texture) printf("couldn't load the placeholder texture\n");
    // --

    // creating an entity
    sprites[0] = (sprite_t){0, 0, 100, 100, placeholder_texture};

    bool running = true;
    
    while(running){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
            }
        }

        glClearColor(0.1, 0.1, 0.1, 1.0); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render
        render_entities(sprites, 1, shader_program);
        //draw_quad(shader_program, placeholder_texture);

        SDL_GL_SwapWindow(window);
    }

    
    SDL_GL_DestroyContext(gl_context);

    SDL_DestroyWindow(window); 
    SDL_Quit();
}

#ifndef AUDIO_H
#define AUDIO_H

#include <SDL3/SDL.h>

#define MAX_SOUNDS 32

typedef struct {
    Uint8* data;
    int len;
} sound_t;

extern sound_t sounds[MAX_SOUNDS];
extern int num_sounds;
extern SDL_AudioStream* audio_stream;
extern SDL_AudioSpec audio_spec;

int init_audio(void);
void cleanup_audio(void);
int load_sound(const char* filename);
void play_sound(int id);

#endif
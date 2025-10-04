#ifndef AUDIO_H
#define AUDIO_H

#include <SDL3/SDL.h>

#define MAX_SOUNDS 16

typedef struct {
    int *data;
    int *len;
} sound_t;

extern sound_t sounds[MAX_SOUNDS];
extern int num_sounds;
extern SDL_AudioStream *audio_stream;
extern SDL_AudioSpec audio_spec;

int init_audio();
void cleanup_audio();
int load_sound(const char *path);
void play_sound(int id);

#endif
#ifndef AUDIO_H
#define AUDIO_H

#include <SDL3/SDL.h>

#define MAX_SOUNDS 32

typedef struct {
    Uint8* data;
    int len;
    int position;
    int looping;
} sound_t;

extern sound_t sounds[MAX_SOUNDS];
extern int num_sounds;

int init_audio(SDL_AudioSpec audio_spec, SDL_AudioStream *audio_stream);
void cleanup_audio(void);
int load_sound(const char* filename, SDL_AudioSpec audio_spec);
void play_sound(int id, int loop, SDL_AudioStream *audio_stream);
void pause_sound(int id, SDL_AudioStream *audio_stream);
void resume_sound(int id, SDL_AudioStream *audio_stream);
void update_audio(SDL_AudioStream *audio_stream, SDL_AudioSpec audio_spec);

#endif
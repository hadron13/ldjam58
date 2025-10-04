#include "audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

sound_t sounds[MAX_SOUNDS];
int num_sounds = 0;
SDL_AudioStream *audio_stream = NULL;
SDL_AudioSpec audio_spec;

// TODO: implement these functions
int init_audio() {

}

void cleanup_audio() {

}

int load_sound(const char *path) {

}

void play_sound(int id) {

}
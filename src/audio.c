#include "audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

sound_t sounds[MAX_SOUNDS];
int num_sounds = 0;
SDL_AudioStream* audio_stream = NULL;
SDL_AudioSpec audio_spec;

int init_audio(void) {
    SDL_zero(audio_spec);
    audio_spec.freq = 44100;
    audio_spec.format = SDL_AUDIO_S16SYS;
    audio_spec.channels = 2;
    audio_spec.samples = 4096;

    audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec, NULL, NULL);
    if (!audio_stream) {
        printf("Couldn't open audio stream: %s\n", SDL_GetError());
        return -1;
    }

    SDL_ResumeAudioStreamDevice(audio_stream);
    return 0;
}

void cleanup_audio(void) {
    if (audio_stream) {
        SDL_DestroyAudioStream(audio_stream);
        audio_stream = NULL;
    }
    for (int i = 0; i < num_sounds; i++) {
        if (sounds[i].data) {
            SDL_free(sounds[i].data);
            sounds[i].data = NULL;
        }
    }
    num_sounds = 0;
}

int load_sound(const char* filename) {
    if (num_sounds >= MAX_SOUNDS) {
        printf("Max sounds reached\n");
        return -1;
    }

    SDL_AudioSpec loaded_spec;
    Uint8* loaded_data;
    Uint32 loaded_len;
    if (!SDL_LoadWAV(filename, &loaded_spec, &loaded_data, &loaded_len)) {
        printf("Couldn't load %s: %s\n", filename, SDL_GetError());
        return -1;
    }

    SDL_AudioCVT cvt;
    if (SDL_BuildAudioCVT(&cvt, loaded_spec.format, loaded_spec.channels, loaded_spec.freq,
                          audio_spec.format, audio_spec.channels, audio_spec.freq) < 0) {
        printf("Couldn't build audio CVT: %s\n", SDL_GetError());
        SDL_free(loaded_data);
        return -1;
    }

    cvt.buf = (Uint8*)malloc(loaded_len * cvt.len_mult);
    if (!cvt.buf) {
        printf("Malloc failed for audio conversion\n");
        SDL_free(loaded_data);
        return -1;
    }

    memcpy(cvt.buf, loaded_data, loaded_len);
    SDL_free(loaded_data);

    if (SDL_ConvertAudio(&cvt) < 0) {
        printf("Couldn't convert audio: %s\n", SDL_GetError());
        free(cvt.buf);
        return -1;
    }

    sounds[num_sounds].data = cvt.buf;
    sounds[num_sounds].len = cvt.len;
    return num_sounds++;
}

void play_sound(int id) {
    if (id < 0 || id >= num_sounds || !sounds[id].data) {
        return;
    }
    SDL_PutAudioStreamData(audio_stream, sounds[id].data, sounds[id].len);
}
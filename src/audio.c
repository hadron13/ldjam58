#include "audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

sound_t sounds[MAX_SOUNDS];
int num_sounds = 0;

int init_audio(SDL_AudioSpec *audio_spec, SDL_AudioStream **audio_stream) {
    SDL_zero(*audio_spec);
    audio_spec->freq = 44100;
    audio_spec->format = SDL_AUDIO_S16;
    audio_spec->channels = 2;

    *audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, audio_spec, NULL, NULL);

    SDL_ResumeAudioStreamDevice(*audio_stream);
    return 0;
}

int load_sound(const char* filename, SDL_AudioSpec *audio_spec) {
    if (num_sounds >= MAX_SOUNDS) {
        printf("Max sounds reached\n");
        return -1;
    }

    SDL_AudioSpec loaded_spec;
    Uint8* loaded_data;
    int loaded_len;
    if (!SDL_LoadWAV(filename, &loaded_spec, &loaded_data, &loaded_len)) {
        printf("Couldn't load %s: %s\n", filename, SDL_GetError());
        return -1;
    }

    Uint8* converted_data = NULL;
    int converted_len = 0;
    if (!SDL_ConvertAudioSamples(&loaded_spec, loaded_data, loaded_len, audio_spec, &converted_data, &converted_len)) {
        printf("Couldn't convert audio: %s\n", SDL_GetError());
        SDL_free(loaded_data);
        return -1;
    }

    SDL_free(loaded_data);

    sounds[num_sounds].data = converted_data;
    sounds[num_sounds].len = converted_len;
    sounds[num_sounds].position = 0;
    sounds[num_sounds].looping = true;
    return num_sounds++;
}

void play_sound(int id, int loop, SDL_AudioStream **audio_stream) {
    if (id < 0 || id >= num_sounds || !sounds[id].data) {
        return;
    }
    SDL_PutAudioStreamData(*audio_stream, sounds[id].data, sounds[id].len);

    sounds[id].position = 0;
    sounds[id].looping = loop;
}

static bool RCS_paused = false;

void pause_sound(int id, SDL_AudioStream **audio_stream, bool *is_paused) {
    if (*is_paused) return;
    *is_paused = true;
    SDL_AudioDeviceID dev = SDL_GetAudioStreamDevice(*audio_stream);
    if (dev != 0) {
        SDL_PauseAudioDevice(dev);
    }
}

void resume_sound(int id, SDL_AudioStream **audio_stream, bool *is_paused) {
    if (!*is_paused) return;
    *is_paused = false;
    SDL_ResumeAudioStreamDevice(*audio_stream);
}

// unsafe nuclear explosions generator \/
void update_audio(int i, SDL_AudioStream **audio_stream, SDL_AudioSpec *audio_spec) {
    if (!*audio_stream) return;

    const int samples_per_chunk = 4096;
    const int bytes_per_sample = SDL_AUDIO_BITSIZE(audio_spec->format) / 8 * audio_spec->channels;
    const int buffer_size = samples_per_chunk * bytes_per_sample;

    // Temp mixing buffer (16-bit signed audio)
    Sint16 *mix_buffer = (Sint16*)SDL_calloc(1, buffer_size);
    if (!mix_buffer) return;

    {
        sound_t *s = &sounds[i];
        if (!s->data) return;

        int remaining = s->len - s->position;
        if (remaining <= 0) {
            if (s->looping) {
                s->position = 0;
                remaining = s->len;
            } else {
                return;
            }
        }

        int to_copy = buffer_size;
        if (to_copy > remaining) {
            to_copy = remaining;
        }

        // Mix: add this sound into the buffer
        Sint16 *src = (Sint16*)(s->data + s->position);
        for (int j = 0; j < to_copy / 2; j++) {
            int mixed = mix_buffer[j] + src[j];
            // clamp to avoid overflow
            if (mixed > 32767) mixed = 32767;
            if (mixed < -32768) mixed = -32768;
            mix_buffer[j] = (Sint16)mixed;
        }

        s->position += to_copy;
    }

    SDL_PutAudioStreamData(*audio_stream, mix_buffer, buffer_size);
    SDL_free(mix_buffer);
}

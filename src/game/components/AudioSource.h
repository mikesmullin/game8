#pragma once

typedef struct Entity Entity;
typedef struct WavReader WavReader;

void AudioSource__play(Entity* entity, WavReader* sound);
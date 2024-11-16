#pragma once

#include "../Engine.h"  // IWYU pragma: keep

typedef struct AudioSourceComponent {
} AudioSourceComponent;

void AudioSource__play(WavReader* sound, Entity* emitter, Entity* listener);
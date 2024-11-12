#pragma once

#include "Types.h"
#include "Wav.h"

void Audio__init();
void Audio__reload();
void Audio__preload();
void Audio__replay(WavReader* src);
void Audio__stop();
void Audio__stream_cb(f32* buffer, int num_frames, int num_channels);
void Audio__shutdown();
#include "AudioSource.h"

void AudioSource__play(WavReader* sound, Entity* emitter, Entity* listener) {
  f32 MAX_HEAR_DIST = 15.0f;
  // only play if listener is near
  if (0 != listener->hear) {
    // calc pan value
    v3 p1_minus_p0, forward, right, up = v3_new(0.0f, 1.0f, 0.0f);
    f32 pan_value;

    // Get the direction vector from player1 to source
    v3_sub(&p1_minus_p0, &listener->tform->pos, &emitter->tform->pos);

    // Calculate forward vector (assuming rot0_y is the yaw in radians)
    forward.x = Math__cosf(Math__DEG2RAD32 * listener->tform->rot3.y);
    forward.y = 0.0f;
    forward.z = -Math__sinf(Math__DEG2RAD32 * listener->tform->rot3.y);
    v3_norm(&forward, &forward);

    // Calculate the right vector (player1's right side)
    v3_cross(&right, &forward, &up);
    v3_norm(&right, &right);

    // Project vector onto the right direction
    pan_value = v3_dot(&right, &p1_minus_p0);

    // Normalize pan value within [-1, 1]
    pan_value = Math__clamp(pan_value / MAX_HEAR_DIST, -1.0f, 1.0f);

    // calc distance/dropoff between source and listener, set volume/pan

    v3 dist;
    v3_sub(&dist, &listener->tform->pos, &emitter->tform->pos);
    f32 d1 = v3_mag2(&dist);
    f32 d2 = Math__map(
        Math__clamp(2, Math__fabsf(d1), MAX_HEAR_DIST),  //
        2,
        MAX_HEAR_DIST,  //
        1,
        0);

    // LOG_DEBUGF("d1 %f, d2 %f, pan %f", d1, d2, pan_value);
    // TODO: find best way to set/modify d2, pan_value during (mixed) playback
    sound->gain = d2;
    Audio__replay(sound);
  }
}
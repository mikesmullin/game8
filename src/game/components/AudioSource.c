#include "AudioSource.h"

#include "../Logic.h"
#include "../common/Audio.h"
#include "../common/Log.h"
#include "../common/Math.h"
#include "../common/Wav.h"

extern Engine__State* g_engine;

void AudioSource__play(Entity* entity, WavReader* sound) {
  f32 MAX_HEAR_DIST = 15.0f;
  Player* player = (Player*)g_engine->logic->player;
  // only play if listener is near
  if (0 != player->base.hear) {
    // calc pan value
    HMM_Vec3 p1_minus_p0, forward, right, up = HMM_V3(0, 1, 0);
    f32 pan_value;

    // Get the direction vector from player to source
    p1_minus_p0 = HMM_SubV3(
        HMM_V3(player->base.tform->pos.x, player->base.tform->pos.y, player->base.tform->pos.z),
        HMM_V3(entity->tform->pos.x, entity->tform->pos.y, entity->tform->pos.z));

    // Calculate forward vector (assuming rot0_y is the yaw in radians)
    f32 rY = player->base.tform->rot.y;
    forward.X = HMM_CosF(rY);
    forward.Y = 0;
    forward.Z = -HMM_SinF(rY);
    forward = HMM_NormV3(forward);

    // Calculate the right vector (player's right side)
    right = HMM_NormV3(HMM_Cross(forward, up));

    // Project vector onto the right direction
    pan_value = HMM_DotV3(
        HMM_V3(right.X, right.Y, right.Z),
        HMM_V3(p1_minus_p0.X, p1_minus_p0.Y, p1_minus_p0.Z));

    // Normalize pan value within [-1, 1]
    pan_value = Math__clamp(pan_value / MAX_HEAR_DIST, -1.0f, 1.0f);

    // calc distance/dropoff between source and listener, set volume/pan

    f32 d1 = HMM_LenV3(HMM_SubV3(
        HMM_V3(player->base.tform->pos.x, player->base.tform->pos.y, player->base.tform->pos.z),
        HMM_V3(entity->tform->pos.x, entity->tform->pos.y, entity->tform->pos.z)));
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
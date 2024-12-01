#include "Rigidbody2D.h"

void Rigidbody2D__move(QuadTree* qt, Entity* entity, Dispatcher__call_t cb) {
  if (0 == entity->rb) return;

  // integrate angular velocity into rotation
  v4 rotationDelta;
  q_fromAxis(
      &rotationDelta,
      &entity->rb->angularVelocity,
      v3_mag(&entity->rb->angularVelocity) * g_engine->deltaTime);
  v4 cp = v4_cp(&entity->tform->rot4);
  q_mul(&entity->tform->rot4, &rotationDelta, &cp);

  f32 ax = Math__fabsf(entity->rb->velocity.x);
  f32 az = Math__fabsf(entity->rb->velocity.z);

  if (ax != 0) {
    u32 xSteps = (u32)(ax * 100) + 1;
    for (u32 i = xSteps; i > 0; i--) {
      f32 xxa = entity->rb->velocity.x;
      f32 xd = xxa * i / xSteps;
      if (entity->tform->pos.y > 1 ||  // flying = noclip
          !Collider__check(qt, entity, entity->tform->pos.x + xd, entity->tform->pos.z, cb)) {
        // apply velocity to position
        v3 xv = v3_new(xd, 0, 0);
        v3 scaled;
        v3_mulS(&scaled, &xv, g_engine->deltaTime);
        v3_add(&entity->tform->pos, &entity->tform->pos, &scaled);

        break;
      } else {
        entity->rb->velocity.x = 0;
      }
    }
  }

  if (az != 0) {
    u32 zSteps = (u32)(az * 100) + 1;
    for (u32 i = zSteps; i > 0; i--) {
      f32 zza = entity->rb->velocity.z;
      f32 zd = zza * i / zSteps;
      if (entity->tform->pos.y > 1 ||  // flying = noclip
          !Collider__check(qt, entity, entity->tform->pos.x, entity->tform->pos.z + zd, cb)) {
        v3 zv = v3_new(0, 0, zd);
        v3 scaled;
        v3_mulS(&scaled, &zv, g_engine->deltaTime);
        v3_add(&entity->tform->pos, &entity->tform->pos, &scaled);

        break;
      } else {
        entity->rb->velocity.z = 0;
      }
    }
  }
}
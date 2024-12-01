#include "Rigidbody2D.h"

void Rigidbody2D__move(QuadTreeNode* qt, Entity* entity, Dispatcher__call_t cb) {
  if (0 == entity->rb) return;

  // apply velocity to position
  v3 scaled;
  v3_mulS(&scaled, &entity->rb->velocity, g_engine->deltaTime);
  v3_add(&entity->tform->pos, &entity->tform->pos, &scaled);

  // integrate angular velocity into rotation
  v4 rotationDelta;
  q_fromAxis(
      &rotationDelta,
      &entity->rb->angularVelocity,
      v3_mag(&entity->rb->angularVelocity) * g_engine->deltaTime);
  v4 cp = v4_cp(&entity->tform->rot4);
  q_mul(&entity->tform->rot4, &rotationDelta, &cp);

  // if (Math__fabsf(entity->rb->xa) < FLT_MIN) entity->rb->xa = 0;
  // if (Math__fabsf(entity->rb->za) < FLT_MIN) entity->rb->za = 0;

  // if (entity->rb->xa != 0) {
  //   u32 xSteps = (u32)(Math__fabsf(entity->rb->xa * 100) + 1);
  //   for (u32 i = xSteps; i > 0; i--) {
  //     f32 xxa = entity->rb->xa;
  //     f32 xd = xxa * i / xSteps;
  //     if (entity->tform->pos.y > 1 ||  // flying = noclip
  //         !Collider__check(qt, entity, entity->tform->pos.x + xd, entity->tform->pos.z, cb)) {
  //       entity->tform->pos.x += xd;
  //       break;
  //     } else {
  //       entity->rb->xa = 0;
  //     }
  //   }
  // }

  // if (entity->rb->za != 0) {
  //   u32 zSteps = (u32)(Math__fabsf(entity->rb->za * 100) + 1);
  //   for (u32 i = zSteps; i > 0; i--) {
  //     f32 zza = entity->rb->za;
  //     f32 zd = zza * i / zSteps;
  //     if (entity->tform->pos.y > 1 ||  // flying = noclip
  //         !Collider__check(qt, entity, entity->tform->pos.x, entity->tform->pos.z + zd, cb)) {
  //       entity->tform->pos.z += zd;
  //       break;
  //     } else {
  //       entity->rb->za = 0;
  //     }
  //   }
  // }
}
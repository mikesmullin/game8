#include "Rigidbody2D.h"

#include <float.h>
#include <math.h>

#include "../Logic.h"
#include "../components/Collider.h"

void Rigidbody2D__move(Entity* entity) {
  if (0 == entity->rb) return;
  if (fabs(entity->rb->xa) < FLT_MIN) entity->rb->xa = 0;
  if (fabs(entity->rb->za) < FLT_MIN) entity->rb->za = 0;

  if (entity->rb->xa != 0) {
    u32 xSteps = (u32)(fabs(entity->rb->xa * 100) + 1);
    for (u32 i = xSteps; i > 0; i--) {
      f32 xxa = entity->rb->xa;
      f32 xd = xxa * i / xSteps;
      if (entity->tform->pos.y > 1 ||  // flying = noclip
          !Collider__check(entity, entity->tform->pos.x + xd, entity->tform->pos.z)) {
        entity->tform->pos.x += xd;
        break;
      } else {
        entity->rb->xa = 0;
      }
    }
  }

  if (entity->rb->za != 0) {
    u32 zSteps = (u32)(fabs(entity->rb->za * 100) + 1);
    for (u32 i = zSteps; i > 0; i--) {
      f32 zza = entity->rb->za;
      f32 zd = zza * i / zSteps;
      if (entity->tform->pos.y > 1 ||  // flying = noclip
          !Collider__check(entity, entity->tform->pos.x, entity->tform->pos.z + zd)) {
        entity->tform->pos.z += zd;
        break;
      } else {
        entity->rb->za = 0;
      }
    }
  }
}
#include "Collider.h"

#include "../Logic.h"
#include "../common/Dispatcher.h"
#include "../common/Geometry.h"
#include "../common/QuadTree.h"

extern Engine__State* g_engine;

// TODO: add a boxcircle checker

bool BoxCollider2D__check(f32 x0, f32 y0, f32 r0, f32 x1, f32 y1, f32 r1) {
  // TODO: use component transforms (then everywhere)

  // TODO: this is not the correct formula for checking boxes

  // Check for overlap using AABB (Axis-Aligned Bounding Box)
  // based on projected a position, and existing b position
  bool overlap_x = (x0 - r0 < x1 + r1) && (x0 + r0 > x1 - r1);
  bool overlap_y = (y0 - r0 < y1 + r1) && (y0 + r0 > y1 - r1);

  // If both x and y axes overlap, a collision is detected
  if (overlap_x && overlap_y) {
    return true;
  }

  // No collision detected, movement allowed
  return false;
}

bool CircleCollider2D__check(f32 x0, f32 y0, f32 r0, f32 x1, f32 y1, f32 r1) {
  // TODO: use component transforms (then everywhere)

  // Check for overlap using AABB (Axis-Aligned Bounding Box)
  // based on projected a position, and existing b position
  bool overlap_x = (x0 - r0 < x1 + r1) && (x0 + r0 > x1 - r1);
  bool overlap_y = (y0 - r0 < y1 + r1) && (y0 + r0 > y1 - r1);

  // If both x and y axes overlap, a collision is detected
  if (overlap_x && overlap_y) {
    return true;
  }

  // No collision detected, movement allowed
  return false;
}

bool Collider__check(Entity* entity, f32 x, f32 y) {
  if (NULL == entity->collider) return false;

  Logic__State* logic = g_engine->logic;
  Level* level = logic->level;

  // use quadtree query to find nearby neighbors
  Rect range = {x, y, 0, 0};
  f32 r0, r1;
  if (BOX_COLLIDER_2D == entity->collider->type) {
    BoxCollider2DComponent* collider = (BoxCollider2DComponent*)entity->collider;
    range.w = collider->hw, range.h = collider->hh;
    r0 = collider->hw;
  } else if (CIRCLE_COLLIDER_2D == entity->collider->type) {
    CircleCollider2DComponent* collider = (CircleCollider2DComponent*)entity->collider;
    range.w = collider->r, range.h = collider->r;
    r0 = collider->r;
  }

  u32 matchCount = 0;
  void* matchData[20];  // TODO: don't limit search results?
  // TODO: query can be the radius of the entity, to shorten this code?
  QuadTreeNode_query(level->qt, range, 20, matchData, &matchCount);
  for (u32 i = 0; i < matchCount; i++) {
    Entity* other = (Entity*)matchData[i];
    if (entity == other) continue;
    if (NULL == other->collider) continue;
    if (TAG_BROKEN & other->tags1) continue;

    if (BOX_COLLIDER_2D == other->collider->type) {
      BoxCollider2DComponent* collider = (BoxCollider2DComponent*)other->collider;
      r1 = collider->hw;
    } else if (CIRCLE_COLLIDER_2D == other->collider->type) {
      CircleCollider2DComponent* collider = (CircleCollider2DComponent*)other->collider;
      r1 = collider->r;
    }

    if (NULL != other->collider) {
      // TODO: use appropriate check: circle_circle, circle_box, box_box
      bool collisionBefore = CircleCollider2D__check(
          entity->tform->pos.x,
          entity->tform->pos.z,
          r0,
          other->tform->pos.x,
          other->tform->pos.z,
          r1);
      bool collisionAfter =
          CircleCollider2D__check(x, y, r0, other->tform->pos.x, other->tform->pos.z, r1);

      if (collisionBefore || collisionAfter) {
        // notify each participant (onenter, onstay, onexit)
        OnCollideParams params = {entity, other, x, y, collisionBefore, collisionAfter, false};
        Dispatcher__call2(other->collider->collide, other, &params);  // notify target
        if (params.noclip) return false;
        Dispatcher__call2(entity->collider->collide, entity, &params);  // notify source
        if (params.noclip) return false;
      }
      if (collisionAfter) return true;
    }
  }

  return false;
}
#pragma once

#include "../common/Types.h"  // IWYU pragma: keep

// TODO: bit-shift on comparison, not in storage
// ie. flags |= 1 << FLAG_ENUM
typedef enum EntityTags1 /* : u64 */ {
  TAG_NONE = 0,
  TAG_LOADED = 1 << 1,
  TAG_WALL = 1 << 2,
  TAG_CAT = 1 << 3,
  TAG_CATSPAWN = 1 << 4,
  TAG_BRICK = 1 << 5,
  TAG_BROKEN = 1 << 6,
  TAG_SKY = 1 << 7,
  TAG_USEABLE = 1 << 8,
  TAG_BLOCKING = 1 << 9,
} EntityTags1;

typedef struct DispatchComponent DispatchComponent;
typedef struct TransformComponent TransformComponent;
typedef struct ColliderComponent ColliderComponent;
typedef struct Rigidbody2DComponent Rigidbody2DComponent;
typedef struct RendererComponent RendererComponent;
typedef struct HealthComponent HealthComponent;
typedef struct AudioSourceComponent AudioSourceComponent;
typedef struct AudioListenerComponent AudioListenerComponent;

typedef struct Entity {
  u32 id;
  u64 tags1;
  bool removed;
  DispatchComponent* dispatch;
  TransformComponent* tform;
  // EventEmitterComponent* event;
  ColliderComponent* collider;
  Rigidbody2DComponent* rb;
  RendererComponent* render;
  HealthComponent* health;
  // // TODO: if we don't need to iterate, these can be moved within subclass
  AudioSourceComponent* audio;
  AudioListenerComponent* hear;
  // EventEmitter* events;
} Entity;

typedef struct OnEntityParams {
  Entity* entity;
} OnEntityParams;

void Entity__init(Entity* entity);

#pragma once

#include <stdbool.h>
#include <stdint.h>
typedef int32_t s32;

typedef struct Level Level;
typedef struct Entity Entity;

Level* Level__alloc();
void Level__init(Level* level);
void Level__preload(Level* level);
void Level__tick(Level* level);
void Level__render(Level* level);
void Level__gui(Level* level);

s32 Level__zsort(void* a, void* b);
Entity* Level__findEntity(Level* level, bool (*findCb)(Entity* e));